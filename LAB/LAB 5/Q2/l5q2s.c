#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int check_anagram(const char *str1, const char *str2) {
    int count[256] = {0};

    while (*str1 && *str2) {
        count[(int)*str1]++;
        count[(int)*str2]--;
        str1++;
        str2++;
    }

    if (*str1 || *str2)
        return 0;

    for (int i = 0; i < 256; i++) {
        if (count[i] != 0)
            return 0;
    }

    return 1;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket to address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Socket binding failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept client connection
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("Accepting connection failed");
            continue;
        }

        // Display client connection info
        printf("Connected to client: %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Receive strings from client
        recv(client_socket, buffer, BUFFER_SIZE, 0);
        char *str1 = strtok(buffer, ",");
        char *str2 = strtok(NULL, ",");

        // Check if strings are anagrams
        int result = check_anagram(str1, str2);

        // Send result to client
        send(client_socket, &result, sizeof(int), 0);

        // Display result on server
        printf("Anagram check result sent to client: %s\n", result ? "Anagrams" : "Not anagrams");

        // Close client socket
        close(client_socket);
    }

    // Close server socket
    close(server_socket);

    return 0;
}

