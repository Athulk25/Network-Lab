// SERVER - Go Back N Protocol

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
    int nextseq = 0;

    while(base < TOTAL_PACKETS)
    {
        while(nextseq < base + WINDOW_SIZE &&
              nextseq < TOTAL_PACKETS)
        {
            sprintf(buffer, "%d", nextseq);

            send(new_sockfd, buffer, sizeof(buffer), 0);

            printf("Packet Sent: %d\n", nextseq);

            nextseq++;
        }

        recv(new_sockfd, buffer, sizeof(buffer), 0);

        if(buffer[0] == 'A')
        {
            int ack = atoi(&buffer[1]);

            printf("ACK Received for Packet %d\n", ack);

            base = ack + 1;
        }
        else if(buffer[0] == 'R')
        {
            int resend = atoi(&buffer[1]);

            printf("Retransmission Requested from Packet %d\n",
                   resend);

            nextseq = resend;
            base = resend;
        }
    }

    printf("\nAll packets transmitted successfully.\n");

    close(new_sockfd);
    close(sockfd);

    return 0;
}

// CLIENT - Go Back N Protocol

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define TOTAL_PACKETS 10

int main()
{
    int sockfd;
    struct sockaddr_in server;

    char buffer[100];

    int expected = 0;
    int lost = 0;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(3033);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&server,
            sizeof(server));

    sprintf(buffer, "REQUEST");

    send(sockfd, buffer, sizeof(buffer), 0);

    printf("Connected to Server...\n");

    while(expected < TOTAL_PACKETS)
    {
        recv(sockfd, buffer, sizeof(buffer), 0);

        int pkt = atoi(buffer);

        printf("\nReceived Packet: %d\n", pkt);

        // Simulate loss of packet 3 only once
        if(pkt == 3 && lost == 0)
        {
            printf("Packet %d Lost/Corrupted\n", pkt);

            sprintf(buffer, "R%d", pkt);

            send(sockfd, buffer, sizeof(buffer), 0);

            lost = 1;

            continue;
        }

        if(pkt == expected)
        {
            printf("Packet %d Accepted\n", pkt);

            sprintf(buffer, "A%d", pkt);

            send(sockfd, buffer, sizeof(buffer), 0);

            printf("ACK Sent for Packet %d\n", pkt);

            expected++;
        }
        else
        {
            printf("Out of Order Packet Discarded: %d\n",
                   pkt);

            sprintf(buffer, "A%d", expected - 1);

            send(sockfd, buffer, sizeof(buffer), 0);
        }
    }

    printf("\nAll packets received successfully.\n");

    close(sockfd);

    return 0;
}
