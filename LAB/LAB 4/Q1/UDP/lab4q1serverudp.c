#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h> // Added for file operations

void handle_registration(int client_socket, struct sockaddr_in client_addr) {
    char name[256];
    char address[256];
    int child_pid = getpid();
    
    recvfrom(client_socket, name, sizeof(name), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
    recvfrom(client_socket, address, sizeof(address), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
    
    char response[512];
    snprintf(response, sizeof(response), "Name: %s\nAddress: %s\nChild PID: %d\n", name, address, child_pid);
    sendto(client_socket, response, strlen(response), 0, (struct sockaddr*)&client_addr, sizeof(client_addr));
}

// Similar functions for handling name and subject...

int main() {
    int server_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // Bind
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Server listening on port 12345...\n");

    while (1) {
        // Accept client connection
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

        int choice;
        recv(client_socket, &choice, sizeof(choice), 0);

        switch (choice) {
            case 1:
                handle_registration(client_socket);
                break;
            case 2:
                handle_name(client_socket);
                break;
            case 3:
                handle_subject(client_socket);
                break;
            default:
                printf("Invalid choice\n");
                break;
        }

        close(client_socket);
    }

    return 0;
}

