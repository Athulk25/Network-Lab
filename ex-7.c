// SERVER - Selective Repeat Protocol

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define WINDOW_SIZE 4
#define TOTAL_PACKETS 10

int main()
{
    int sockfd, new_sockfd;
    struct sockaddr_in server, client;
    socklen_t len;

    char buffer[100];
    int ack[TOTAL_PACKETS] = {0};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(3033);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr *)&server, sizeof(server));

    listen(sockfd, 1);

    printf("Starting Server...\n");

    len = sizeof(client);

    new_sockfd = accept(sockfd, (struct sockaddr *)&client, &len);

    recv(new_sockfd, buffer, sizeof(buffer), 0);

    int base = 0;

    while(base < TOTAL_PACKETS)
    {
        printf("\nSending Window:\n");

        for(int i = base; i < base + WINDOW_SIZE && i < TOTAL_PACKETS; i++)
        {
            if(ack[i] == 0)
            {
                sprintf(buffer, "%d", i);
                send(new_sockfd, buffer, sizeof(buffer), 0);

                printf("Packet Sent: %d\n", i);
            }
        }

        for(int i = base; i < base + WINDOW_SIZE && i < TOTAL_PACKETS; i++)
        {
            recv(new_sockfd, buffer, sizeof(buffer), 0);

            if(buffer[0] == 'A')
            {
                int pkt = atoi(&buffer[1]);

                printf("ACK Received for Packet %d\n", pkt);

                ack[pkt] = 1;
            }
        }

        while(ack[base] == 1 && base < TOTAL_PACKETS)
        {
            base++;
        }
    }

    printf("\nAll packets transmitted successfully.\n");

    close(new_sockfd);
    close(sockfd);

    return 0;
}

// CLIENT - Selective Repeat Protocol

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define WINDOW_SIZE 4
#define TOTAL_PACKETS 10

int main()
{
    int sockfd;
    struct sockaddr_in server;

    char buffer[100];

    int received[TOTAL_PACKETS] = {0};

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(3033);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&server, sizeof(server));

    sprintf(buffer, "REQUEST");
    send(sockfd, buffer, sizeof(buffer), 0);

    printf("Connected to Server...\n");

    while(1)
    {
        recv(sockfd, buffer, sizeof(buffer), 0);

        int pkt = atoi(buffer);

        if(pkt >= TOTAL_PACKETS)
            break;

        printf("\nReceived Packet: %d\n", pkt);

        // Simulate loss of packet 2 only once
        if(pkt == 2 && received[2] == 0)
        {
            printf("Packet %d Lost/Corrupted\n", pkt);

            received[2] = -1;

            continue;
        }

        if(received[pkt] != 1)
        {
            received[pkt] = 1;

            printf("Packet %d Accepted\n", pkt);
        }

        sprintf(buffer, "A%d", pkt);

        send(sockfd, buffer, sizeof(buffer), 0);

        printf("ACK Sent for Packet %d\n", pkt);

        int complete = 1;

        for(int i = 0; i < TOTAL_PACKETS; i++)
        {
            if(received[i] != 1)
            {
                complete = 0;
                break;
            }
        }

        if(complete)
            break;
    }

    printf("\nAll packets received successfully.\n");

    close(sockfd);

    return 0;
}
