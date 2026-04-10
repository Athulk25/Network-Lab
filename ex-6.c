//SERVER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

int main()
{
    int sockfd, new_sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server, client;
    socklen_t len;

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(9009);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd,(struct sockaddr*)&server,sizeof(server));
    listen(sockfd,5);

    len = sizeof(client);
    new_sockfd = accept(sockfd,(struct sockaddr*)&client,&len);

    char msg[50] = "server message ";
    char buffer[50];
    int flag = 0;

    fd_set set;
    struct timeval timeout;

    for(int i=0;i<5;i++)
    {
        bzero(buffer,sizeof(buffer));
        strcpy(buffer,msg);
        buffer[strlen(msg)] = i + '0';

        printf("Message sent to client: %s\n",buffer);

        FD_ZERO(&set);
        FD_SET(new_sockfd,&set);

        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        write(new_sockfd,buffer,sizeof(buffer));

        int rv = select(new_sockfd+1,&set,NULL,NULL,&timeout);

        if(rv==0)
        {
            printf("Resending message\n");
            i--;
            continue;
        }

        read(new_sockfd,buffer,sizeof(buffer));
        printf("Message from client: %s\n",buffer);
    }

    close(new_sockfd);
    close(sockfd);
}

//CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    memset(&server,0,sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(9009);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd,(struct sockaddr*)&server,sizeof(server));

    char msg1[50] = "acknowledgement-0";
    char msg2[50] = "acknowledgement-1";
    char buffer[100];

    for(int i=0;i<5;i++)
    {
        bzero(buffer,sizeof(buffer));
        read(sockfd,buffer,sizeof(buffer));

        printf("Message received from server: %s\n",buffer);
        printf("Acknowledgement sent\n");

        if(i%2==0)
            write(sockfd,msg1,sizeof(msg1));
        else
            write(sockfd,msg2,sizeof(msg2));
    }

    close(sockfd);
}

//SIMULATION

#include <stdio.h>
#include <stdlib.h>

int info;
int seq;
int ak;
int t = 5;
int disconnect = 0;
int errorframe = 1, errorack = 1;
char turn = 's';

struct frame
{
    int info;
    int seq;
}p;

void sender();
void receiver();

void main()
{
    p.info = 0;
    p.seq = 0;

    while(!disconnect)
    {
        sender();
        for(int k=1;k<=1000000;k++);
        receiver();
    }
}

void sender()
{
    static int flag = 0;

    if(turn == 's')
    {
        if(errorack == 0)
        {
            printf("SENDER: Sent packet with seq NO: %d\n",p.seq);
            errorframe = rand()%4;
            printf("%s\n",(errorframe==0?"Error while sending packet":""));
            turn='r';
        }
        else
        {
            if(flag==1)
                printf("SENDER Received ACK for packet %d\n",ak);

            if(p.seq==5)
            {
                disconnect=1;
                return;
            }

            p.info=p.info+1;
            p.seq=p.seq+1;

            printf("SENDER sent packet with seq NO: %d\n",p.seq);
            errorframe=rand()%4;
            printf("%s\n",(errorframe==0?"Error while sending packet":""));
            turn='r';
            flag=1;
        }
    }
    else
    {
        t--;
        printf("SENDER time reducing\n");

        if(t==0)
        {
            turn='s';
            errorack=0;
            t=5;
        }
    }
}

void receiver()
{
    static int frexp = 1;

    if(turn=='r')
    {
        if(errorframe!=0)
        {
            if(p.seq==frexp)
            {
                printf("RECEIVER: Received packet with seq %d\n",p.seq);
                ak=p.seq;
                frexp=frexp+1;
                turn='s';
                errorack=rand()%4;
                printf("%s\n",(errorack==0?"Error while sending ACK":""));
            }
            else
            {
                printf("RECEIVER: Duplicated packet with seq %d\n",frexp-1);
                ak=frexp-1;
                turn='s';
                errorack=rand()%4;
                printf("%s\n",(errorack==0?"Error while sending ACK":""));
            }
        }
    }
}
