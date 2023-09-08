#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define myport 1234
#define MAX_LEN 100

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr, client_addr;
    socklen_t len;
    int ns, recb, sntb;
    char buff[MAX_LEN];
    char buff2[MAX_LEN];  // Added buffer for parent input
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
    printf("Socket accepted.\n");

    int pid = fork();
    if (pid == -1) {
        perror("Fork error");
        close(ns);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        if (pid > 0) {
            // parent
            printf("Parent - Type message: ");
            scanf("%s", buff2);

            sntb = send(ns, buff2, sizeof(buff2), 0);
            if (sntb == -1) {
                perror("Message Sending Failed");
                close(ns);
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            if (strcmp(buff2, "BYE") == 0)
                break;
        } else {
            // child
            recb = recv(ns, buff, sizeof(buff), 0);
            if (recb == -1) {
                perror("Message Receiving Failed");
                close(ns);
                close(sockfd);
                exit(EXIT_FAILURE);
            }
            if (strcmp(buff, "BYE") == 0)
                break;
            printf("Child - Message Received: %s\n", buff);
        }
    }

    close(ns);
    close(sockfd);

    return 0;
}

