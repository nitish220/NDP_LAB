#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>

#define MAX_LEN 100
#define x 1234

int main() {
    int s, r, recb, sntb;
    int sa;
    socklen_t len;
    struct sockaddr_in server, client;
    char buff[50];
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        printf("\nSocket creation error.");
        exit(0);
    }
    printf("\nSocket created.");
    server.sin_family = AF_INET;
    server.sin_port = htons(x);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    sa = sizeof(server);
    len = sizeof(server);

    while (1) {
        printf("\n\n");
        printf("Type Message (enter 'stop' to exit): ");
        scanf("%s", buff);

        sntb = sendto(s, buff, sizeof(buff), 0, (struct sockaddr *)&server, len);
        if (sntb == -1) {
            close(s);
            printf("\nMessage sending Failed");
            exit(0);
        }
        if (!strcmp(buff, "stop"))
            break;
    }
    close(s);
}
