#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENTS 2
#define BUFFER_SIZE 1024

void handle_client(int client_socket, struct sockaddr_in client_address) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    recv(client_socket, buffer, sizeof(buffer), 0);
    
    printf("Received from %s:%d: %s\n",
           inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);  //ip adress, port, data
    
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);
    
    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }
    
    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(12345);
    
    // Bind
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        close(server_socket);
        return EXIT_FAILURE;
    }
    
    // Listen
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        close(server_socket);
        return EXIT_FAILURE;
    }
    
    printf("Server listening on port %d...\n", ntohs(server_address.sin_port));
    
    // Handle clients
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }
        
        // Fork to handle client
        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("Fork failed");
            close(client_socket);
            continue;
        }
        
        if (child_pid == 0) {  // Child process
            close(server_socket);
            handle_client(client_socket, client_address);
            exit(EXIT_SUCCESS);
        } else {  // Parent process
            close(client_socket);
        }
    }
    
    close(server_socket);
    
    return EXIT_SUCCESS;
}

