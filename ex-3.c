//SERVER

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char buffer[100];
    int k;
    socklen_t len;
    int sockfd, new_sockfd;
    struct sockaddr_in server, client;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(3003); // Port number in network byte order

    k = bind(sockfd, (struct sockaddr*)&server, sizeof(server));
    if (k == -1) {
        perror("Error in binding");
        exit(EXIT_FAILURE);
    }

    k = listen(sockfd, 5);
    if (k == -1) {
        perror("Error in listening");
        exit(EXIT_FAILURE);
    }

    len = sizeof(client);
    new_sockfd = accept(sockfd, (struct sockaddr*)&client, &len);
    if (new_sockfd == -1) {
        perror("Error in temporary socket creation");
        exit(EXIT_FAILURE);
    }

    while (1) {
        k = recv(new_sockfd, buffer, sizeof(buffer) - 1, 0);
        if (k == -1) {
            perror("Error in receiving");
            exit(EXIT_FAILURE);
        } 
        else if (k == 0) {
            printf("Connection closed by client\n");
            break;
        }

        buffer[k] = '\0';
        printf("Message from client is: %s", buffer);

        if (strcmp(buffer, "exit\n") == 0) {
            printf("Exiting the program.\n");
            break;
        }

        printf("Enter data to Client (type 'exit' to quit): ");
        fgets(buffer, sizeof(buffer), stdin);

        k = send(new_sockfd, buffer, strlen(buffer), 0);
        if (k == -1) {
            perror("Error in sending");
            exit(EXIT_FAILURE);
        }
    }

    close(new_sockfd);
    close(sockfd);
    return 0;
}

//CLIENT

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    char buffer[100];
    int k;
    int sockfd;
    struct sockaddr_in server;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(3003);

    k = connect(sockfd, (struct sockaddr*)&server, sizeof(server));
    if (k == -1) {
        perror("Error in connecting to server");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("\nEnter data to Server (type 'exit' to quit): ");
        fgets(buffer, sizeof(buffer), stdin);

        size_t data_len = strlen(buffer);
        while (data_len > 0) {
            k = send(sockfd, buffer, data_len, 0);
            if (k == -1) {
                perror("Error in sending");
                exit(EXIT_FAILURE);
            }
            data_len -= k;
        }

        if (strcmp(buffer, "exit\n") == 0) {
            printf("Exiting the program.\n");
            break;
        }

        memset(buffer, 0, sizeof(buffer));
        size_t received_len = 0;

        do {
            k = recv(sockfd, buffer + received_len, sizeof(buffer) - received_len - 1, 0);

            if (k == -1) {
                perror("Error in receiving");
                exit(EXIT_FAILURE);
            }
            else if (k == 0) {
                printf("Connection closed by server\n");
                exit(EXIT_FAILURE);
            }

            received_len += k;

        } while (buffer[received_len - 1] != '\n');

        buffer[received_len - 1] = '\0';
        printf("Message from server is: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
