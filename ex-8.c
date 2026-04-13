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
    int sockfd, new_sockfd, windowStart = 1, windowCurrent = 1, windowEnd = 4;
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

    new_sockfd = accept(sockfd, (struct sockaddr *)&client, &len);

    recv(new_sockfd, buffer, 100, 0);

    fcntl(new_sockfd, F_SETFL, O_NONBLOCK);

    printf("\nReceived a request from client. Sending packets one by one...");

    do
    {
        if (windowCurrent != windowEnd)
        {
            itoa(windowCurrent, buffer);
            send(new_sockfd, buffer, 100, 0);

            printf("\nPacket Sent: %d\n", windowCurrent);
            windowCurrent++;
        }

        recv(new_sockfd, buffer, 100, 0);

        if (buffer[0] == 'R')
        {
            printf("\n** Received a RETRANSMIT packet. Resending packet...");
            send(new_sockfd, buffer, 100, 0);

            windowCurrent = atoi(&buffer[1]);
        }

        if (buffer[0] == 'A')
        {
            windowStart = windowStart + (windowStart - oldWindowStart);
        }

    } while (windowCurrent != windowEnd);

    printf("\nAll packets sent successfully. Closing connection.\n");

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

int main()
{
    int sockfd, firstTime = 1, currentPacket, wait = 3;
    char buffer[100], digit[2];

    struct sockaddr_in server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = 3033;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("\nStarting up...");
    int len = sizeof(server);

    printf("\nEstablishing connection to server...");
    connect(sockfd, (struct sockaddr *)&server, len);

    sprintf(buffer, "REQUEST");
    send(sockfd, buffer, strlen(buffer), 0);

    do
    {
        recv(sockfd, buffer, 100, 0);

        currentPacket = atoi(buffer);

        printf("\nGot packet: %d", currentPacket);

        if (currentPacket == 3 && firstTime)
        {
            printf("\n** Simulation: Packet data corrupted or incomplete.");
            printf("\n** Sending RETRANSMIT for packet 1.");

            memset(buffer, 0, sizeof(buffer));
            sprintf(buffer, "R1");

            send(sockfd, buffer, strlen(buffer), 0);

            firstTime = 0;
        }
        else
        {
            wait--;

            if (!wait)
            {
                printf("\n** Packet Accepted -> Sending ACK\n");

                wait = 3;

                sprintf(buffer, "A");

                digit[0] = (char)(currentPacket + 48);
                digit[1] = '\0';

                strcat(buffer, digit);

                send(sockfd, buffer, strlen(buffer), 0);
            }
        }

    } while (currentPacket != 9);

    printf("\nAll packets received. Exiting.\n");

    close(sockfd);

    return 0;
}
