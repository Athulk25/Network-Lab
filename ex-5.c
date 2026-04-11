//SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define MAX_BUFFER_SIZE 1024
#define MATRIX_SIZE 3

void matrix_multiply(int A[MATRIX_SIZE][MATRIX_SIZE],
                     int B[MATRIX_SIZE][MATRIX_SIZE],
                     int result[MATRIX_SIZE][MATRIX_SIZE]) {
    int i, j, k;

    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            result[i][j] = 0;
            for (k = 0; k < MATRIX_SIZE; k++) {
                result[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server, client;
    char buffer[MAX_BUFFER_SIZE];

    int matrixA[MATRIX_SIZE][MATRIX_SIZE];
    int matrixB[MATRIX_SIZE][MATRIX_SIZE];
    int result[MATRIX_SIZE][MATRIX_SIZE];

    socklen_t len;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started and listening on port %d...\n", PORT);

    while (1) {
        len = sizeof(client);

        if (recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0,
                     (struct sockaddr *)&client, &len) == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        memcpy(matrixA, buffer, sizeof(matrixA));

        if (recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0,
                     (struct sockaddr *)&client, &len) == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        memcpy(matrixB, buffer, sizeof(matrixB));

        matrix_multiply(matrixA, matrixB, result);

        memcpy(buffer, result, sizeof(result));

        if (sendto(sockfd, buffer, sizeof(result), 0,
                   (struct sockaddr *)&client, len) == -1) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }
    }

    close(sockfd);
    return 0;
}

//CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define MAX_BUFFER_SIZE 1024
#define MATRIX_SIZE 3

void print_matrix(int matrix[MATRIX_SIZE][MATRIX_SIZE]) {
    int i, j;
    for (i = 0; i < MATRIX_SIZE; i++) {
        for (j = 0; j < MATRIX_SIZE; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server;
    char buffer[MAX_BUFFER_SIZE];

    int matrixA[MATRIX_SIZE][MATRIX_SIZE];
    int matrixB[MATRIX_SIZE][MATRIX_SIZE];
    int result[MATRIX_SIZE][MATRIX_SIZE];

    socklen_t len;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server, 0, sizeof(server));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    printf("Enter elements of matrix A (%dx%d):\n", MATRIX_SIZE, MATRIX_SIZE);
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            scanf("%d", &matrixA[i][j]);
        }
    }

    printf("Enter elements of matrix B (%dx%d):\n", MATRIX_SIZE, MATRIX_SIZE);
    for (int i = 0; i < MATRIX_SIZE; i++) {
        for (int j = 0; j < MATRIX_SIZE; j++) {
            scanf("%d", &matrixB[i][j]);
        }
    }

    memcpy(buffer, matrixA, sizeof(matrixA));
    sendto(sockfd, buffer, sizeof(matrixA), 0,
           (struct sockaddr *)&server, sizeof(server));

    memcpy(buffer, matrixB, sizeof(matrixB));
    sendto(sockfd, buffer, sizeof(matrixB), 0,
           (struct sockaddr *)&server, sizeof(server));

    len = sizeof(server);

    recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0,
             (struct sockaddr *)&server, &len);

    memcpy(result, buffer, sizeof(result));

    printf("\nMatrix A:\n");
    print_matrix(matrixA);

    printf("\nMatrix B:\n");
    print_matrix(matrixB);

    printf("\nResult:\n");
    print_matrix(result);

    close(sockfd);
    return 0;
}
