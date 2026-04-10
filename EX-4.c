//udpserver.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAXLINE 1024

void reverseString(char *str)
{
    int length = strlen(str);
    int start = 0;
    int end = length - 1;

    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr, cliaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (bind(sockfd, (const struct sockaddr *)&servaddr,
             sizeof(servaddr)) < 0 ) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    int len, n;
    len = sizeof(cliaddr);

    n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                 MSG_WAITALL, (struct sockaddr *)&cliaddr,
                 &len);

    buffer[n] = '\0';
    printf("Original string from client: %s\n", buffer);

    reverseString(buffer);

    sendto(sockfd, (const char *)buffer, strlen(buffer),
           MSG_CONFIRM, (const struct sockaddr *)&cliaddr,
           len);

    printf("Reversed string sent to client.\n");
    return 0;
}


//udpclient.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
    int sockfd;
    char buffer[MAXLINE];
    struct sockaddr_in servaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    printf("Enter a string to reverse: ");
    fgets(buffer, MAXLINE, stdin);

    sendto(sockfd, (const char *)buffer, strlen(buffer),
           MSG_CONFIRM, (const struct sockaddr *)&servaddr,
           sizeof(servaddr));

    printf("String sent to server.\n");

    int n = recvfrom(sockfd, (char *)buffer, MAXLINE,
                     MSG_WAITALL, NULL, NULL);

    buffer[n] = '\0';
    printf("Reversed string from server: %s\n", buffer);

    close(sockfd);
    return 0;
}
