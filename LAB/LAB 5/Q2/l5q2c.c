#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int result;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    // Input two strings
    char str1[BUFFER_SIZE], str2[BUFFER_SIZE];
    printf("Enter string 1: ");
    fgets(str1, BUFFER_SIZE, stdin);
    printf("Enter string 2: ");
    fgets(str2, BUFFER_SIZE, stdin);

    // Send strings to server
    snprintf(buffer, BUFFER_SIZE, "%s,%s", str1, str2);
    send(client_socket, buffer, BUFFER_SIZE, 0);

    // Receive result from server
    recv(client_socket, &result, sizeof(int), 0);

    // Display result
    printf("Server says: %s\n", result ? "Anagrams" : "Not anagrams");

    // Close socket
    close(client_socket);

    return 0;
}

