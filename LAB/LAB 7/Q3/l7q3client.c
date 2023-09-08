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
    char filename[MAX_MSG_LEN];
    char response[MAX_MSG_LEN];

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

    printf("Connected to File Processing Server on %s:%d\n", SERVER_IP, PORT);

    printf("Enter the filename: ");
    fgets(filename, sizeof(filename), stdin);

    // Remove newline character from the filename
    size_t len = strlen(filename);
    if (len > 0 && filename[len - 1] == '\n') {
        filename[len - 1] = '\0';
    }

    // Send the filename to the server
    send(client_socket, filename, strlen(filename), 0);

    // Receive response from the server
    memset(response, 0, sizeof(response));
    recv(client_socket, response, sizeof(response), 0);

    printf("Server Response: %s\n", response);

    close(client_socket);
    return 0;
}

