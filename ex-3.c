// tcpserver.c
#include "sys/socket.h"
#include "netinet/in.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <fcntl.h>
#include <unistd.h>

int main() {
    char buf[100];
    int k;
    socklen_t len;
    int sock_desc, temp_sock_desc;
    struct sockaddr_in server, client;

    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_desc == -1) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(3003); // Port number in network byte order

    k = bind(sock_desc, (struct sockaddr*)&server, sizeof(server));
    if (k == -1) {
        perror("Error in binding");
        exit(EXIT_FAILURE);
    }

    k = listen(sock_desc, 5);
    if (k == -1) {
        perror("Error in listening");
        exit(EXIT_FAILURE);
    }

    len = sizeof(client);
    temp_sock_desc = accept(sock_desc, (struct sockaddr*)&client, &len);
    if (temp_sock_desc == -1) {
        perror("Error in temporary socket creation");
        exit(EXIT_FAILURE);
    }

    while (1) {
        k = recv(temp_sock_desc, buf, sizeof(buf) - 1, 0);
        if (k == -1) {
            perror("Error in receiving");
            exit(EXIT_FAILURE);
        } 
        else if (k == 0) {
            printf("Connection closed by client\n");
            break;
        }

        buf[k] = '\0';
        printf("Message from client is: %s", buf);

        if (strcmp(buf, "exit\n") == 0) {
            printf("Exiting the program.\n");
            break;
        }

        printf("Enter data to Client (type 'exit' to quit): ");
        fgets(buf, sizeof(buf), stdin);

        k = send(temp_sock_desc, buf, strlen(buf), 0);
        if (k == -1) {
            perror("Error in sending");
            exit(EXIT_FAILURE);
        }
    }

    close(temp_sock_desc);
    close(sock_desc);
    return 0;
}

// tcpclient.c
#include "sys/socket.h"
#include "netinet/in.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include <fcntl.h>
#include <unistd.h>

int main() {
    char buf[100];
    int k;
    int sock_desc;
    struct sockaddr_in server;

    sock_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_desc == -1) {
        perror("Error in socket creation");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(3003);

    k = connect(sock_desc, (struct sockaddr*)&server, sizeof(server));
    if (k == -1) {
        perror("Error in connecting to server");
        exit(EXIT_FAILURE);
    }

    while (1) {
        printf("\nEnter data to Server (type 'exit' to quit): ");
        fgets(buf, sizeof(buf), stdin);

        size_t data_len = strlen(buf);
        while (data_len > 0) {
            k = send(sock_desc, buf, data_len, 0);
            if (k == -1) {
                perror("Error in sending");
                exit(EXIT_FAILURE);
            }
            data_len -= k;
        }

        if (strcmp(buf, "exit\n") == 0) {
            printf("Exiting the program.\n");
            break;
        }

        memset(buf, 0, sizeof(buf));
        size_t received_len = 0;

        do {
            k = recv(sock_desc, buf + received_len,
                     sizeof(buf) - received_len - 1, 0);

            if (k == -1) {
                perror("Error in receiving");
                exit(EXIT_FAILURE);
            }
            else if (k == 0) {
                printf("Connection closed by server\n");
                exit(EXIT_FAILURE);
            }

            received_len += k;

        } while (buf[received_len - 1] != '\n');

        buf[received_len - 1] = '\0';
        printf("Message from server is: %s\n", buf);
    }

    close(sock_desc);
    return 0;
}
