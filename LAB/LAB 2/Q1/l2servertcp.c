#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h> // Added for file operations

#define myport 1234
#define MAX_LEN 100

void replaceAll(char* str, const char* oldWord, const char* newWord) {
    char *pos, temp[1000];
    int index = 0;
    int owlen;

    owlen = strlen(oldWord);
    while ((pos = strstr(str, oldWord)) != NULL) {
        strcpy(temp, str);
        index = pos - str;
        str[index] = '\0';
        strcat(str, newWord);
        strcat(str, temp + index + owlen);
    }
}

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;
    int ns, recb, sntb;
    char buff[MAX_LEN];
    char fil[50];

    if (sockfd == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Initialize the server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(myport);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind the socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Ready for listening
    if (listen(sockfd, 10) == -1) {
        perror("Listen error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("The server is ready for listening.\n");

    len = sizeof(client_addr);

    ns = accept(sockfd, (struct sockaddr*)&client_addr, &len);
    if (ns == -1) {
        perror("Accept error");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("\nSocket accepting.\n");

    recb = recv(ns, buff, sizeof(buff), 0);
    if (recb == -1) {
        perror("Message Receiving Failed");
        close(sockfd);
        close(ns);
        exit(EXIT_FAILURE);
    }
    printf("\nFile Name Received!\n");

    if (access(buff, F_OK) != -1) {
        // File exists
        strcpy(fil, buff);
        strcpy(buff, "File exists");
    } else {
        // File doesn't exist
        strcpy(buff, "File does not exist!");
    }

    sntb = send(ns, buff, sizeof(buff), 0);
    if (sntb == -1) {
        perror("Message Sending Failed");
        close(sockfd);
        close(ns);
        exit(EXIT_FAILURE);
    }
    if (strcmp(buff, "File does not exist!") == 0) {
        close(sockfd);
        close(ns);
        exit(EXIT_SUCCESS); // Exit with success status since the file doesn't exist
    }

    int ch = 0;
    while (ch != 4) {
        recb = recv(ns, buff, sizeof(buff), 0);
        if (recb == -1) {
            perror("Message Receiving Failed");
            close(sockfd);
            close(ns);
            exit(EXIT_FAILURE);
        }
        ch = buff[0];
        int i, n, n1, n2, j;
        char str[50], str1[50], str2[50];
        char strTempData[MAX_LEN];
        char** strData = NULL;
        int noOfLines = 0;
        
        switch (ch) {
            case 1:
                printf("\nSearching..\n");
                n = buff[1];
                for (i = 0; i < n; i++)
                    str[i] = buff[i + 2];
                str[n] = '\0';
                FILE* fp;
                int line_num = 1;
                int find_result = 0;
                char temp[512];
                if ((fp = fopen(fil, "r")) == NULL) {
                    printf("\nFile not found");
                
                    close(ns);
                    exit(0);
                }
                while (fgets(temp, 512, fp) != NULL) {
                    if ((strstr(temp, str)) != NULL) {
                        find_result++;
                    }
                    line_num++;
                }
                if (fp) {
                    fclose(fp);
                }
                buff[0] = find_result;
                sntb = send(ns, buff, sizeof(buff), 0);
                if (sntb == -1) {
                    printf("\nMessage Sending Failed");
                
                    close(ns);
                    exit(0);
                }
                break;

            case 2:
                n1 = buff[1];
                i = 2;
                for (j = 0; j < n1; j++) {
                    str1[j] = buff[i];
                    i++;
                }
                str1[j] = '\0';
                recb = recv(ns, buff, sizeof(buff), 0);
                if (recb == -1) {
                    printf("\nMessage Recieving Failed");
          
                    close(ns);
                    exit(0);
                }
                n = buff[1];
                i = 2;
                for (j = 0; j < n; j++) {
                    str2[j] = buff[i];
                    i++;
                }
                str2[j] = '\0';
                printf("\nReplacing %s with %s..\n", str1, str2);
                FILE* fPtr;
                FILE* fTemp;
                char buffer[1000];
                fPtr = fopen(fil, "r");
                fTemp = fopen("replace.tmp", "w");
                if (fPtr == NULL || fTemp == NULL) {
                    /* Unable to open file hence exit */
                    printf("\nUnable to open file.\n");
                    printf("Please check whether file exists and you have read/write privilege.\n");
                    exit(0);
                }
                while ((fgets(buffer, 1000, fPtr)) != NULL) {
                    // Replace all occurrence of word from current line
                    replaceAll(buffer, str1, str2);

                    // After replacing write it to temp file.
                    fputs(buffer, fTemp);
                }

                /* Close all files to release resource */
                fclose(fPtr);
                fclose(fTemp);

                /* Delete original source file */
                remove(fil);

                /* Rename temp file as original file */
                rename("replace.tmp", fil);
                strcpy(buff, "Replace finished!");
                sntb = send(ns, buff, sizeof(buff), 0);
                if (sntb == -1) {
                    printf("\nMessage Sending Failed");
                  
                    close(ns);
                    exit(0);
                }
                break;

            case 3:
                printf("\nOrdering file..\n");

                FILE* ptrFileLog = NULL;
                FILE* ptrSummary = NULL;

                if ((ptrFileLog = fopen(fil, "r")) == NULL) {
                    fprintf(stderr, "Error: Could not open %s\n", fil);
                    return 1;
                }
                if ((ptrSummary = fopen("temp.txt", "a")) == NULL) {
                    fprintf(stderr, "Error: Could not open temp.txt\n");
                    return 1;
                }

                // Read and store in a string list.
                while (fgets(strTempData, MAX_LEN, ptrFileLog) != NULL) {
                    // Remove the trailing newline character
                    if (strchr(strTempData, '\n'))
                        strTempData[strlen(strTempData) - 1] = '\0';
                    strData = (char**)realloc(strData, sizeof(char**) * (noOfLines + 1));
                    strData[noOfLines] = (char*)calloc(MAX_LEN, sizeof(char));
                    strcpy(strData[noOfLines], strTempData);
                    noOfLines++;
                }
                // Sort the array.
                for (i = 0; i < (noOfLines - 1); ++i) {
                    for (j = 0; j < (noOfLines - i - 1); ++j) {
                        if (strcmp(strData[j], strData[j + 1]) > 0) {
                            strcpy(strTempData, strData[j]);
                            strcpy(strData[j], strData[j + 1]);
                            strcpy(strData[j + 1], strTempData);
                        }
                    }
                }
                // Write it to outfile. file.
                for (i = 0; i < noOfLines; i++)
                    fprintf(ptrSummary, "%s\n", strData[i]);
                // free each string
                for (i = 0; i < noOfLines; i++)
                    free(strData[i]);
                // free string list.
                free(strData);
                remove(fil);
                rename("temp.txt", fil);
                fclose(ptrFileLog);
                fclose(ptrSummary);
                strcpy(buff, "Ordering done!");
                sntb = send(ns, buff, sizeof(buff), 0);
                if (sntb == -1) {
                    printf("\nMessage Sending Failed");
               
                    close(ns);
                    exit(0);
                }
                break;

            case 4:
                ch = 4;
                break;
        }
    }
    
    close(ns);
    close(sockfd);
    return 0;
}

