//SERVER

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    int sockfd, new_sockfd;
    char filename[2000], message[2000];
    FILE *fp;

    struct sockaddr_in server, client;
    socklen_t len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("Could not create socket\n");
        return 1;
    }

    printf("Socket created\n");

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Bind failed\n");
        return 1;
    }

    printf("Bind done\n");

    listen(sockfd, 3);
    printf("Waiting for incoming connections..\n");

    len = sizeof(struct sockaddr_in);

    new_sockfd = accept(sockfd, (struct sockaddr *)&client, &len);

    if (new_sockfd < 0)
    {
        perror("Accept failed");
        return 1;
    }

    printf("Connection accepted\n");

    while (1)
    {
        recv(new_sockfd, filename, 2000, 0);

        fp = fopen(filename, "r");

        if (fp == NULL)
        {
            strcpy(message, "File Not Found\n");
            send(new_sockfd, message, 2000, 0);
        }
        else
        {
            while (fgets(message, 2000, fp))
            {
                send(new_sockfd, message, 2000, 0);
                printf("%s", message);
            }
        }

        fclose(fp);
        strcpy(message, "EOF");
        send(new_sockfd, message, 2000, 0);
    }

    return 0;
}

//CLIENT

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main(int argc, char *argv[])
{
    int sockfd;
    char message[2000], filename[2000];
    struct sockaddr_in server;
    FILE *fp;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        printf("Could not create socket\n");
        return 1;
    }

    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        printf("Connect error\n");
        return 1;
    }

    printf("Connected\n");

    printf("File Name: ");
    scanf("%s", filename);

    if (send(sockfd, filename, strlen(filename), 0) < 0)
    {
        printf("Send failed\n");
        return 1;
    }

    fp = fopen("backup", "w");

    while (recv(sockfd, message, 2000, 0))
    {
        if (strcmp(message, "EOF") == 0)
            break;

        fputs(message, fp);
        printf("%s", message);
    }

    fclose(fp);

    return 0;
}

