#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);

    // Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Server IP
    server_addr.sin_port = htons(12345);  // Server port

    int choice;
    printf("Enter your choice:\n1. Registration Number\n2. Name of the Student\n3. Subject Code\n");
    scanf("%d", &choice);
    sendto(client_socket, &choice, sizeof(choice), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    switch (choice) {
        case 1: {
            char name[256];
            char address[256];

            printf("Enter Name: ");
            scanf(" %[^\n]s", name);
            sendto(client_socket, name, sizeof(name), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

            printf("Enter Address: ");
            scanf(" %[^\n]s", address);
            sendto(client_socket, address, sizeof(address), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            break;
        }
        case 2:
            // Sending student name, nothing to input
            break;
        case 3: {
            char subject_code[256];

            printf("Enter Subject Code: ");
            scanf(" %[^\n]s", subject_code);
            sendto(client_socket, subject_code, sizeof(subject_code), 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
            break;
        }
        default:
            printf("Invalid choice\n");
            break;
    }

    // Receive and print server response
    char response[1024];
    recvfrom(client_socket, response, sizeof(response), 0, NULL, NULL);
    printf("Server Response:\n%s\n", response);

    close(client_socket);
    return 0;
}

