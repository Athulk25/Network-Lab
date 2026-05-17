// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MAX 1024

int main() {

    int sockfd;
    char buffer[MAX];
    struct sockaddr_in server, client;
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));

    printf("Server Waiting...\n\n");

    len = sizeof(client);

    while (1) {

        recvfrom(sockfd, buffer, MAX, 0,
                 (struct sockaddr *)&client, &len);

        int frame = atoi(buffer);

        printf("Received Frame: %d\n", frame);

        // Simulate ACK loss for frame 3
        if (frame == 3) {
            static int lost = 0;

            if (lost == 0) {
                printf("ACK Lost\n\n");
                lost = 1;
                continue;
            }
        }

        sprintf(buffer, "%d", frame);

        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&client, len);

        printf("ACK Sent: %d\n\n", frame);
    }

    close(sockfd);

    return 0;
}

// client.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define MAX 1024

int main() {

    int sockfd;
    char buffer[MAX];
    struct sockaddr_in server;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t len = sizeof(server);

    for (int i = 1; i <= 5; i++) {

        while (1) {

            sprintf(buffer, "%d", i);

            sendto(sockfd, buffer, strlen(buffer), 0,
                   (struct sockaddr *)&server, len);

            printf("Frame Sent: %d\n", i);

            fd_set readfds;
            struct timeval tv;

            FD_ZERO(&readfds);
            FD_SET(sockfd, &readfds);

            tv.tv_sec = 3;
            tv.tv_usec = 0;

            int rv = select(sockfd + 1, &readfds,
                            NULL, NULL, &tv);

            if (rv == 0) {
                printf("Timeout Occurred...\n");
                printf("Retransmitting Frame: %d\n\n", i);
            }
            else {

                recvfrom(sockfd, buffer, MAX, 0,
                         NULL, NULL);

                printf("ACK Received: %s\n\n", buffer);

                break;
            }
        }
    }

    close(sockfd);

    return 0;
}
