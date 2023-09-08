#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define MAX_LEN 100
#define myport 1234

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_port = htons(myport);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int size = sizeof(struct sockaddr);
    connect(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr));

    char buffer[MAX_LEN];
    ssize_t send_bytes, receive_bytes;

    printf("\nEnter file name: ");
    scanf("%s", buffer);

    send_bytes = send(sockfd, buffer, sizeof(buffer), 0);
    if (send_bytes == -1) {
        perror("Error sending message");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    receive_bytes = recv(sockfd, buffer, sizeof(buffer), 0);
    if (receive_bytes == -1) {
        perror("Error receiving message");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("%s\n\n", buffer);

    if (strcmp(buffer, "File does not exist!") == 0) {
        close(sockfd);
        exit(EXIT_SUCCESS);
    }

    int choice = 0;
    while (choice != 4) {
        printf("1. Search\n2. Replace\n3. Reorder\n4. Exit\nEnter your choice: ");
        scanf("%d", &choice);
        buffer[0] = choice;
        char search_str[50], replace_str[50];
        int str_length, i, j;

        switch (choice) {
            case 1:
                printf("Enter string to search: ");
                scanf("%s", search_str);
                str_length = strlen(search_str);
                buffer[1] = str_length;
                for (i = 0; i < str_length; i++)
                    buffer[i + 2] = search_str[i];
                buffer[i + 2] = '\0';

                send_bytes = send(sockfd, buffer, sizeof(buffer), 0);
                if (send_bytes == -1) {
                    perror("Error sending message");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }

                receive_bytes = recv(sockfd, buffer, sizeof(buffer), 0);
                if (receive_bytes == -1) {
                    perror("Error receiving message");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }

                printf("Word found %d times!\n", buffer[0]);
                break;

            case 2:
                printf("Enter string to search and replace: ");
                scanf("%s", search_str);
                str_length = strlen(search_str);
                buffer[1] = str_length;
                for (i = 0; i < str_length; i++)
                    buffer[i + 2] = search_str[i];
                buffer[i + 2] = '\0';

                send_bytes = send(sockfd, buffer, sizeof(buffer), 0);
                if (send_bytes == -1) {
                    perror("Error sending message");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }

                printf("Enter new string: ");
                scanf("%s", replace_str);
                str_length = strlen(replace_str);
                buffer[1] = str_length;
                i = 2;
                for (j = 0; j < str_length; j++) {
                    buffer[i] = replace_str[j];
                    i++;
                }
                buffer[i] = '\0';

                send_bytes = send(sockfd, buffer, sizeof(buffer), 0);
                if (send_bytes == -1) {
                    perror("Error sending message");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }

                receive_bytes = recv(sockfd, buffer, sizeof(buffer), 0);
                if (receive_bytes == -1) {
                    perror("Error receiving message");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }

                printf("%s\n", buffer);
                break;

            case 3:
                send_bytes = send(sockfd, buffer, sizeof(buffer), 0);
                if (send_bytes == -1) {
                    perror("Error sending message");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }

                receive_bytes = recv(sockfd, buffer, sizeof(buffer), 0);
                if (receive_bytes == -1) {
                    perror("Error receiving message");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }

                printf("%s\n", buffer);
                break;

            case 4:
                send_bytes = send(sockfd, buffer, sizeof(buffer), 0);
                if (send_bytes == -1) {
                    perror("Error sending message");
                    close(sockfd);
                    exit(EXIT_FAILURE);
                }
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }
    }

    close(sockfd);
    return 0;
}

