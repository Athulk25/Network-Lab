//SERVER

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define WINDOW_SIZE 4
#define MAX_PACKET 9

void itoa(int number, char numberString[])
{
    numberString[0] = (char)(number + 48);
    numberString[1] = '\0';
}

int main()
{
    int sockfd, new_sockfd, windowStart = 1, windowEnd = WINDOW_SIZE, windowCurrent;
    char buffer[100];
    socklen_t len;
    struct sockaddr_in server, client;
    int all_sent = 0;

    server.sin_family = AF_INET;
    server.sin_port = htons(3033);
    server.sin_addr.s_addr = INADDR_ANY;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("\nStarting up...");

    int k = bind(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (k == -1)
    {
        printf("Error in binding\n");
        return -1;
    }

    len = sizeof(client);
    listen(sockfd, 1);
    new_sockfd = accept(sockfd, (struct sockaddr *)&client, &len);
    recv(new_sockfd, buffer, 100, 0);
    printf("\nReceived a request from client. Sending packets (Go-Back-N)...");

    while (!all_sent)
    {
        // Send packets in current window
        for (windowCurrent = windowStart; windowCurrent <= windowEnd && windowCurrent <= MAX_PACKET; windowCurrent++)
        {
            itoa(windowCurrent, buffer);
            send(new_sockfd, buffer, 100, 0);
            printf("\nPacket Sent: %d", windowCurrent);
        }

        // Wait for ACK or RETRANSMIT
        recv(new_sockfd, buffer, 100, 0);
        if (buffer[0] == 'R')
        {
            int pkt = atoi(&buffer[1]);
            printf("\n** Received a RETRANSMIT for packet %d. Resending window from %d...", pkt, pkt);
            windowStart = pkt; // Go back
            windowEnd = windowStart + WINDOW_SIZE - 1;
        }
        else if (buffer[0] == 'A')
        {
            int ack = atoi(&buffer[1]);
            printf("\n** Received ACK for packet %d.", ack);
            windowStart = ack + 1;
            windowEnd = windowStart + WINDOW_SIZE - 1;
            if (ack >= MAX_PACKET)
                all_sent = 1;
        }
    }

    printf("\nAll packets sent and acknowledged. Closing connection.\n");
    close(new_sockfd);
    close(sockfd);
    return 0;
}

//CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define WINDOW_SIZE 4
#define MAX_PACKET 9

int main()
{
    int sockfd, firstTime = 1, currentPacket;
    char buffer[100], digit[2];
    struct sockaddr_in server;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(3033);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    printf("\nStarting up...");
    int len = sizeof(server);
    printf("\nEstablishing connection to server...");
    connect(sockfd, (struct sockaddr *)&server, len);
    sprintf(buffer, "REQUEST");
    send(sockfd, buffer, strlen(buffer), 0);
    int expected = 1;
    int all_received = 0;
    while (!all_received)
    {
        memset(buffer, 0, sizeof(buffer));
        recv(sockfd, buffer, 100, 0);
        currentPacket = atoi(buffer);
        printf("\nGot packet: %d", currentPacket);
        if (currentPacket == 3 && firstTime)
        {
            printf("\n** Simulation: Packet data corrupted or incomplete.");
            printf("\n** Sending RETRANSMIT for packet 3.");
            sprintf(buffer, "R3");
            send(sockfd, buffer, strlen(buffer), 0);
            firstTime = 0;
        }
        else if (currentPacket == expected)
        {
            printf("\n** Packet Accepted -> Sending ACK\n");
            sprintf(buffer, "A%d",currentPacket);
            send(sockfd, buffer, strlen(buffer), 0);
            expected++;
            if (currentPacket >= MAX_PACKET)
                all_received = 1;
        }
        else
        {
            // Out of order or duplicate, send ACK for last accepted
            sprintf(buffer, "A%d", expected-1);
            send(sockfd, buffer, strlen(buffer), 0);
        }
    }
    printf("\nAll packets received. Exiting.\n");
    close(sockfd);
    return 0;
}
