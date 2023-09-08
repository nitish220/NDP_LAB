#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>

void handle_registration(int client_socket) {
    char name[256];
    char address[256];
    int child_pid = getpid();
    
    recv(client_socket, name, sizeof(name), 0);
    recv(client_socket, address, sizeof(address), 0);
    
    char response[512];
    printf(response, sizeof(response), "Name: %s\nAddress: %s\nChild PID: %d\n", name, address, child_pid);
    send(client_socket, response, strlen(response), 0);
}

void handle_name(int client_socket) {
    char enrollment[] = "Dept: Computer Science\nSemester: 3\nSection: A\nCourses: CS101, Math202";
    int child_pid = getpid();
    char response[256];

    snprintf(response, sizeof(response), "Enrollment Details:\n%s\nChild PID: %d\n", enrollment, child_pid);
    send(client_socket, response, strlen(response), 0);
}

void handle_subject(int client_socket) {
    char marks[] = "Subject: Math202\nMarks Obtained: 95";
    int child_pid = getpid();
    char response[256];

    snprintf(response, sizeof(response), "Marks Details:\n%s\nChild PID: %d\n", marks, child_pid);
    send(client_socket, response, strlen(response), 0);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // Bind
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);
    bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Listen
    listen(server_socket, 5);
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

    close(server_socket);
    return 0;
}

