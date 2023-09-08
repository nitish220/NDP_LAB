#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h> // Added for isalpha and toupper

#define PORT 12345
#define MAX_MSG_LEN 1024

void processFile(char* filename, int client_socket) {
    char buffer[MAX_MSG_LEN];
    FILE *file = fopen(filename, "r");
    FILE *uppercaseFile = fopen("uppercase.txt", "w");
    FILE *digitsFile = fopen("digits.txt", "w");

    if (!file) {
        perror("File not found");
        send(client_socket, "File not found", sizeof("File not found"), 0);
        return;
    }

    pid_t child_pid = fork();

    if (child_pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (child_pid == 0) {
        // Child process: Convert text to uppercase
        while (fgets(buffer, sizeof(buffer), file)) {
            for (int i = 0; buffer[i] != '\0'; i++) {
                buffer[i] = toupper(buffer[i]);
            }
            fputs(buffer, uppercaseFile);
        }
        fclose(uppercaseFile);
        exit(EXIT_SUCCESS); // Child process should exit
    } else {
        // Parent process: Replace letters with digits
        int digit;
        while (fgets(buffer, sizeof(buffer), file)) {
            for (int i = 0; buffer[i] != '\0'; i++) {
                if (isalpha(buffer[i])) {
                    digit = buffer[i] - (islower(buffer[i]) ? 'a' - 1 : 'A' - 1);
                    fprintf(digitsFile, "%d", digit);
                } else {
                    fputc(buffer[i], digitsFile);
                }
            }
        }
        fclose(digitsFile);
        wait(NULL);
    }

    fclose(file);
    send(client_socket, "File processed successfully", sizeof("File processed successfully"), 0);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char filename[MAX_MSG_LEN];

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_socket, 5) == -1) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("File Processing Server listening on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("accept");
            continue;
        }

        memset(filename, 0, sizeof(filename));
        ssize_t recv_size = recv(client_socket, filename, sizeof(filename), 0);

        if (recv_size <= 0) {
            perror("recv");
            close(client_socket);
            continue;
        }

        printf("Received request for file: %s\n", filename);

        processFile(filename, client_socket);

        close(client_socket);
    }

    close(server_socket);
    return 0;
}

