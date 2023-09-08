#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define MAX_MSG_LEN 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[MAX_MSG_LEN];

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connected to Chat Server on %s:%d\n", SERVER_IP, PORT);

    while (1) {
        printf("You: ");
        fgets(message, sizeof(message), stdin);
        send(client_socket, message, strlen(message), 0);

        memset(message, 0, sizeof(message));
        recv(client_socket, message, sizeof(message), 0);

        printf("Server: %s", message);
    }

    close(client_socket);
    return 0;
}

