#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 12345

typedef struct {
    char domain[50];
    char ip_address[20];
} DNSRecord;

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

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

    printf("DNS Server listening on port %d...\n", PORT);

    // Load DNS records from database
    DNSRecord records[3];
    strcpy(records[0].domain, "www.example.com");
    strcpy(records[0].ip_address, "192.168.1.1");
    strcpy(records[1].domain, "mail.example.com");
    strcpy(records[1].ip_address, "192.168.1.2");
    strcpy(records[2].domain, "ftp.example.com");
    strcpy(records[2].ip_address, "192.168.1.3");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("accept");
            continue;
        }

        char domain[50];
        recv(client_socket, domain, sizeof(domain), 0);

        char response[20] = "Not found";
        for (int i = 0; i < 3; i++) {
            if (strcmp(domain, records[i].domain) == 0) {
                strcpy(response, records[i].ip_address);
                break;
            }
        }

        send(client_socket, response, sizeof(response), 0);

        close(client_socket);
    }

    close(server_socket);
    return 0;
}

