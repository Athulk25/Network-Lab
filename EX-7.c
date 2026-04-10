//SERVER
#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<unistd.h>
#include<arpa/inet.h>

// structure definition for designing the packet.
struct frame {
    int packet[40];
};

// structure definition for accepting the acknowledgement.
struct ack {
    int acknowledge[40];
};

int main() {
    int serversocket;
    struct sockaddr_in serveraddr, clientaddr;
    socklen_t len;

    struct frame f1;
    struct ack acknowledgement;

    int windowsize, totalpackets, totalframes;
    int i = 0, j = 0, framesend = 0, k, l, buffer;
    char req[50];

    serversocket = socket(AF_INET, SOCK_DGRAM, 0);

    bzero((char*)&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(5018);
    serveraddr.sin_addr.s_addr = INADDR_ANY;

    bind(serversocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    bzero((char*)&clientaddr, sizeof(clientaddr));
    len = sizeof(clientaddr);

    // connection establishment
    printf("\nwaiting for client connection");
    recvfrom(serversocket, req, sizeof(req), 0,
             (struct sockaddr*)&clientaddr, &len);

    printf("\nThe client connection obtained\t%s\n", req);

    // sending request for windowsize
    printf("\nSending request for window size\n");
    sendto(serversocket, "REQUEST FOR WINDOWSIZE",
           sizeof("REQUEST FOR WINDOWSIZE"), 0,
           (struct sockaddr*)&clientaddr, sizeof(clientaddr));

    // obtaining windowsize
    printf("Waiting for the window size\n");
    recvfrom(serversocket, (char*)&windowsize, sizeof(windowsize), 0,
             (struct sockaddr*)&clientaddr, &len);

    printf("\nThe window size obtained as:\t %d \n", windowsize);

    printf("\nObtaining packets from network layer\n");
    totalpackets = windowsize * 5;
    totalframes = 5;

    printf("\nTotal packets obtained :%d\n", totalpackets);
    printf("\nTotal frames or windows to be transmitted :%d\n", totalframes);

    // sending details to client
    sendto(serversocket, (char*)&totalpackets, sizeof(totalpackets), 0,
           (struct sockaddr*)&clientaddr, sizeof(clientaddr));
    recvfrom(serversocket, req, sizeof(req), 0,
             (struct sockaddr*)&clientaddr, &len);

    sendto(serversocket, (char*)&totalframes, sizeof(totalframes), 0,
           (struct sockaddr*)&clientaddr, sizeof(clientaddr));
    recvfrom(serversocket, req, sizeof(req), 0,
             (struct sockaddr*)&clientaddr, &len);

    printf("\nPress enter to start the process\n");
    fgets(req, 2, stdin);

    while(i < totalpackets) {

        bzero((char*)&f1, sizeof(f1));
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

        sendto(serversocket, (char*)&f1, sizeof(f1), 0,
               (struct sockaddr*)&clientaddr, sizeof(clientaddr));

        printf("Waiting for the acknowledgment\n");

        recvfrom(serversocket, (char*)&acknowledgement,
                 sizeof(acknowledgement), 0,
                 (struct sockaddr*)&clientaddr, &len);

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

    close(serversocket);
}

//CLIENT

#include<stdio.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

struct frame {
    int packet[40];
};

struct ack {
    int acknowledge[40];
};

int main() {
    int clientsocket;
    struct sockaddr_in serveraddr;
    socklen_t len;

    struct hostent *server;
    struct frame f1;
    struct ack acknowledgement;

    int windowsize, totalpackets, totalframes;
    int i = 0, j = 0, framesreceived = 0, k, l, buffer;
    char req[50];

    clientsocket = socket(AF_INET, SOCK_DGRAM, 0);

    bzero((char*)&serveraddr, sizeof(serveraddr));

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(5018);

    server = gethostbyname("127.0.0.1");

    bcopy((char*)server->h_addr,
          (char*)&serveraddr.sin_addr.s_addr,
          sizeof(server->h_addr));

    printf("sending request to the server\n");

    sendto(clientsocket, "HI IAM CLIENT",
           sizeof("HI IAM CLIENT"), 0,
           (struct sockaddr*)&serveraddr,
           sizeof(serveraddr));

    printf("\nWaiting for reply\n");

    recvfrom(clientsocket, req, sizeof(req), 0,
             (struct sockaddr*)&serveraddr, &len);

    printf("\nThe server has to send :\t%s\n", req);

    printf("\nEnter the window size\n");
    scanf("%d", &windowsize);

    printf("\nSending window size\n");
    sendto(clientsocket, (char*)&windowsize, sizeof(windowsize), 0,
           (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    recvfrom(clientsocket, (char*)&totalpackets,
             sizeof(totalpackets), 0,
             (struct sockaddr*)&serveraddr, &len);

    printf("\nTotal packets are: %d\n", totalpackets);

    sendto(clientsocket, "RECEIVED", sizeof("RECEIVED"), 0,
           (struct sockaddr*)&serveraddr, sizeof(serveraddr));

    recvfrom(clientsocket, (char*)&totalframes,
             sizeof(totalframes), 0,
             (struct sockaddr*)&serveraddr, &len);

    printf("\nTotal number of frames are: %d\n", totalframes);

    sendto(clientsocket, "RECEIVED", sizeof("RECEIVED"), 0,
           (struct sockaddr*)&serveraddr, sizeof(serveraddr));

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

        recvfrom(clientsocket, (char*)&f1, sizeof(f1), 0,
                 (struct sockaddr*)&serveraddr, &len);

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

        sendto(clientsocket, (char*)&acknowledgement,
               sizeof(acknowledgement), 0,
               (struct sockaddr*)&serveraddr,
               sizeof(serveraddr));
    }

    printf("\nAll frames received successfully\n");
    printf("Closing connection with the server\n");

    close(clientsocket);
}
