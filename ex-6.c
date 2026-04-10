//Stop-and-Wait ARQ — Server (TCP)

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/select.h>

int main()
{
    int s_sock, c_sock;
    s_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server, other;
    socklen_t add;

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(9009);
    server.sin_addr.s_addr = INADDR_ANY;

    bind(s_sock,(struct sockaddr*)&server,sizeof(server));
    listen(s_sock,5);

    add = sizeof(other);
    c_sock = accept(s_sock,(struct sockaddr*)&other,&add);

    char msg[50] = "server message ";
    char buff[50];
    int flag = 0;

    fd_set set;
    struct timeval timeout;

    for(int i=0;i<5;i++)
    {
        bzero(buff,sizeof(buff));
        strcpy(buff,msg);
        buff[strlen(msg)] = i + '0';

        printf("Message sent to client: %s\n",buff);

        FD_ZERO(&set);
        FD_SET(c_sock,&set);

        timeout.tv_sec = 2;
        timeout.tv_usec = 0;

        write(c_sock,buff,sizeof(buff));

        int rv = select(c_sock+1,&set,NULL,NULL,&timeout);

        if(rv==0)
        {
            printf("Resending message\n");
            i--;
            continue;
        }

        read(c_sock,buff,sizeof(buff));
        printf("Message from client: %s\n",buff);
    }

    close(c_sock);
    close(s_sock);
}

//Stop-and-Wait ARQ — Client

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>

int main()
{
    int c_sock;
    c_sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in client;
    memset(&client,0,sizeof(client));

    client.sin_family = AF_INET;
    client.sin_port = htons(9009);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(c_sock,(struct sockaddr*)&client,sizeof(client));

    char msg1[50] = "acknowledgement-0";
    char msg2[50] = "acknowledgement-1";
    char buff[100];

    for(int i=0;i<5;i++)
    {
        bzero(buff,sizeof(buff));
        read(c_sock,buff,sizeof(buff));

        printf("Message received from server: %s\n",buff);
        printf("Acknowledgement sent\n");

        if(i%2==0)
            write(c_sock,msg1,sizeof(msg1));
        else
            write(c_sock,msg2,sizeof(msg2));
    }

    close(c_sock);
}

//Stop-and-Wait ARQ Simulation Program

#include<stdio.h>
#include<stdlib.h>

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
