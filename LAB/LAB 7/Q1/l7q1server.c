#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 12345
#define MAX_CLIENTS 5

typedef struct {
    char username[50];
    char encrypted_password[50];
    double balance;
} User;

User users[MAX_CLIENTS]; // Simulated user database

void initialize_users() {
strcpy(users[0].username, "user1");
    strcpy(users[0].encrypted_password, "xwv1"); // Encrypted version of "abc1"
    users[0].balance = 1000.0;

    strcpy(users[1].username, "user2");
    strcpy(users[1].encrypted_password, "def2"); // Encrypted version of "abc2"
    users[1].balance = 2000.0;

}

// Function to perform Caesar cipher encryption
void caesar_encrypt(char *str, int shift) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = 'a' + (str[i] - 'a' + shift) % 26;
        }
    }
}

int authenticate_user(int client_socket) {
    char username[50];
    char password[50];

    recv(client_socket, username, sizeof(username), 0);
    recv(client_socket, password, sizeof(password), 0);

    // Perform Caesar cipher encryption on the received password
    caesar_encrypt(password, 3);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strcmp(username, users[i].username) == 0) {
            if (strcmp(password, users[i].encrypted_password) == 0) {
                return i; // Authentication successful, return the user index
            } else {
                return -1; // Incorrect password
            }
        }
    }

    return -2; // Incorrect username
}

void handle_client(int client_socket) {
    int user_index = authenticate_user(client_socket);

    if (user_index == -2) {
        char response[] = "Incorrect Username";
        send(client_socket, response, sizeof(response), 0);
    } else if (user_index == -1) {
        char response[] = "Incorrect Password";
        send(client_socket, response, sizeof(response), 0);
    } else {
        // Successful authentication
        char response[] = "Login Successful. Welcome!";
        send(client_socket, response, sizeof(response), 0);

        // Handle banking transactions here
    }

    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    initialize_users();

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
    if (listen(server_socket, MAX_CLIENTS) == -1) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Bank Server listening on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == -1) {
            perror("accept");
            continue;
        }

        handle_client(client_socket);
    }

    close(server_socket);
    return 0;
}

