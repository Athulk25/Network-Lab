//SERVER

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

// structure definition for designing the packet.
struct frame {
    int packet[40];
};

// structure definition for accepting the acknowledgement.
struct ack {
    int acknowledge[40];
};

int main() {
    int sockfd;
    struct sockaddr_in server, client;
    socklen_t len;

    struct frame f1;
    struct ack acknowledgement;

    int windowsize, totalpackets, totalframes;
    int i = 0, j = 0, framesend = 0, k, l, buffer;
    char req[50];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(5018);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&server, sizeof(server));

    bzero(&client, sizeof(client));
    len = sizeof(client);

    // connection establishment
    printf("\nwaiting for client connection");
    recvfrom(sockfd, req, sizeof(req), 0,
             (struct sockaddr*)&client, &len);

    printf("\nThe client connection obtained\t%s\n", req);

    // sending request for windowsize
    printf("\nSending request for window size\n");
    sendto(sockfd, "REQUEST FOR WINDOWSIZE",
           sizeof("REQUEST FOR WINDOWSIZE"), 0,
           (struct sockaddr*)&client, sizeof(client));

    // obtaining windowsize
    printf("Waiting for the window size\n");
    recvfrom(sockfd, (char*)&windowsize, sizeof(windowsize), 0,
             (struct sockaddr*)&client, &len);

    printf("\nThe window size obtained as:\t %d \n", windowsize);

    printf("\nObtaining packets from network layer\n");
    totalpackets = windowsize * 5;
    totalframes = 5;

    printf("\nTotal packets obtained :%d\n", totalpackets);
    printf("\nTotal frames or windows to be transmitted :%d\n", totalframes);

    // sending details to client
    sendto(sockfd, (char*)&totalpackets, sizeof(totalpackets), 0,
           (struct sockaddr*)&client, sizeof(client));
    recvfrom(sockfd, req, sizeof(req), 0,
             (struct sockaddr*)&client, &len);

    sendto(sockfd, (char*)&totalframes, sizeof(totalframes), 0,
           (struct sockaddr*)&client, sizeof(client));
    recvfrom(sockfd, req, sizeof(req), 0,
             (struct sockaddr*)&client, &len);

    printf("\nPress enter to start the process\n");
    fgets(req, 2, stdin);

    while(i < totalpackets) {

        bzero(&f1, sizeof(f1));
        printf("\nInitializing the transmit buffer\n");

        buffer = i;
        printf("\nThe frame to be send is %d with packets:", framesend);

        j = 0;

        while(j < windowsize && i < totalpackets) {
            printf("%d", i);
            f1.packet[j] = i;
            j++;
            i++;
        }

        printf("\nsending frame %d\n", framesend);

        sendto(sockfd, (char*)&f1, sizeof(f1), 0,
               (struct sockaddr*)&client, sizeof(client));

        printf("Waiting for the acknowledgment\n");

        recvfrom(sockfd, (char*)&acknowledgement,
                 sizeof(acknowledgement), 0,
                 (struct sockaddr*)&client, &len);

        j = 0;
        k = 0;
        l = buffer;

        while(j < windowsize && l < totalpackets) {
            if(acknowledgement.acknowledge[j] == -1) {
                printf("\nnegative acknowledgement received for packet:%d\n",
                       f1.packet[j]);

                printf("\nRetransmitting from packet:%d\n",
                       f1.packet[j]);

                i = f1.packet[j];
                k = 1;
                break;
            }
            j++;
            l++;
        }

        if(k == 0) {
            printf("\nPositive acknowledgement received for all packets "
                   "within the frame:%d\n", framesend);
            framesend++;
            printf("\npress enter to proceed\n");
            fgets(req, 2, stdin);
        }
    }

    printf("\nAll frames sends successfully\n");
    printf("Closing connection with the client\n");

    close(sockfd);
}

//CLIENT

#include <stdio.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

struct frame {
    int packet[40];
};

struct ack {
    int acknowledge[40];
};

int main() {
    int sockfd;
    struct sockaddr_in server;
    socklen_t len;

    struct hostent *host;
    struct frame f1;
    struct ack acknowledgement;

    int windowsize, totalpackets, totalframes;
    int i = 0, j = 0, framesreceived = 0, k, l, buffer;
    char req[50];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&server, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(5018);

    host = gethostbyname("127.0.0.1");

    bcopy(host->h_addr,
      &server.sin_addr.s_addr,
      sizeof(host->h_addr));

    printf("sending request to the server\n");

    sendto(sockfd, "HI IAM CLIENT",
           sizeof("HI IAM CLIENT"), 0,
           (struct sockaddr*)&server,
           sizeof(server));

    printf("\nWaiting for reply\n");

    recvfrom(sockfd, req, sizeof(req), 0,
             (struct sockaddr*)&server, &len);

    printf("\nThe server has to send :\t%s\n", req);

    printf("\nEnter the window size\n");
    scanf("%d", &windowsize);

    printf("\nSending window size\n");
    sendto(sockfd, (char*)&windowsize, sizeof(windowsize), 0,
           (struct sockaddr*)&server, sizeof(server));

    recvfrom(sockfd, (char*)&totalpackets,
             sizeof(totalpackets), 0,
             (struct sockaddr*)&server, &len);

    printf("\nTotal packets are: %d\n", totalpackets);

    sendto(sockfd, "RECEIVED", sizeof("RECEIVED"), 0,
           (struct sockaddr*)&server, sizeof(server));

    recvfrom(sockfd, (char*)&totalframes,
             sizeof(totalframes), 0,
             (struct sockaddr*)&server, &len);

    printf("\nTotal number of frames are: %d\n", totalframes);

    sendto(sockfd, "RECEIVED", sizeof("RECEIVED"), 0,
           (struct sockaddr*)&server, sizeof(server));

    printf("\nStarting the process of receiving\n");

    while(i < totalpackets) {

        printf("\nInitializing the received buffer\n");

        printf("\nThe expected frame is %d with packets:",
               framesreceived);

        j = 0;
        buffer = i;

        while(j < windowsize && i < totalpackets) {
            printf("%d", i);
            i++;
            j++;
        }

        printf("\nWaiting for the frame\n");

        recvfrom(sockfd, (char*)&f1, sizeof(f1), 0,
                 (struct sockaddr*)&server, &len);

        printf("\nReceived frame %d\n", framesreceived);

        j = 0;
        l = buffer;
        k = 0;

        while(j < windowsize && l < totalpackets) {

            printf("\nPacket: %d\n", f1.packet[j]);

            scanf("%d", &acknowledgement.acknowledge[j]);

            if(acknowledgement.acknowledge[j] == -1) {
                if(k == 0)
                    i = f1.packet[j];
                k = 1;
            }

            j++;
            l++;
        }

        framesreceived++;

        sendto(sockfd, (char*)&acknowledgement,
               sizeof(acknowledgement), 0,
               (struct sockaddr*)&server,
               sizeof(server));
    }

    printf("\nAll frames received successfully\n");
    printf("Closing connection with the server\n");

    close(sockfd);
}
