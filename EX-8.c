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

void itoa(int number, char numberString[])
{
    numberString[0] = (char)(number + 48);
    numberString[1] = '\0';
}

int main()
{
    int sockfd, newSockFd, size, windowStart = 1, windowCurrent = 1, windowEnd = 4;
    int oldWindowStart, flag;
    char buffer[100];
    socklen_t len;

    struct sockaddr_in server, client;

    server.sin_family = AF_INET;
    server.sin_port = 3033;
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

    newSockFd = accept(sockfd, (struct sockaddr *)&client, &len);

    recv(newSockFd, buffer, 100, 0);

    fcntl(newSockFd, F_SETFL, O_NONBLOCK);

    printf("\nReceived a request from client. Sending packets one by one...");

    do
    {
        if (windowCurrent != windowEnd)
        {
            itoa(windowCurrent, buffer);
            send(newSockFd, buffer, 100, 0);

            printf("\nPacket Sent: %d\n", windowCurrent);
            windowCurrent++;
        }

        recv(newSockFd, buffer, 100, 0);

        if (buffer[0] == 'R')
        {
            printf("\n** Received a RETRANSMIT packet. Resending packet...");
            send(newSockFd, buffer, 100, 0);

            windowCurrent = atoi(&buffer[1]);
        }

        if (buffer[0] == 'A')
        {
            windowStart = windowStart + (windowStart - oldWindowStart);
        }

    } while (windowCurrent != windowEnd);

    printf("\nAll packets sent successfully. Closing connection.\n");

    close(newSockFd);
    close(sockfd);

    return 0;
}

//CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main()
{
    int sockfd, size, firstTime = 1, currentPacket, wait = 3;
    char data[100], digit[2];

    struct sockaddr_in client;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    client.sin_family = AF_INET;
    client.sin_port = 3033;
    client.sin_addr.s_addr = INADDR_ANY;

    printf("\nStarting up...");
    size = sizeof(client);

    printf("\nEstablishing connection to server...");
    connect(sockfd, (struct sockaddr *)&client, size);

    sprintf(data, "REQUEST");
    send(sockfd, data, strlen(data), 0);

    do
    {
        recv(sockfd, data, 100, 0);

        currentPacket = atoi(data);

        printf("\nGot packet: %d", currentPacket);

        if (currentPacket == 3 && firstTime)
        {
            printf("\n** Simulation: Packet data corrupted or incomplete.");
            printf("\n** Sending RETRANSMIT for packet 1.");

            memset(data, 0, sizeof(data));
            sprintf(data, "R1");

            send(sockfd, data, strlen(data), 0);

            firstTime = 0;
        }
        else
        {
            wait--;

            if (!wait)
            {
                printf("\n** Packet Accepted -> Sending ACK\n");

                wait = 3;

                sprintf(data, "A");

                digit[0] = (char)(currentPacket + 48);
                digit[1] = '\0';

                strcat(data, digit);

                send(sockfd, data, strlen(data), 0);
            }
        }

    } while (currentPacket != 9);

    printf("\nAll packets received. Exiting.\n");

    close(sockfd);

    return 0;
}
