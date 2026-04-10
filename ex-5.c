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
    int server_socket;
    struct sockaddr_in server_address, client_address;
    char buffer[MAX_BUFFER_SIZE];

    int matrixA[MATRIX_SIZE][MATRIX_SIZE];
    int matrixB[MATRIX_SIZE][MATRIX_SIZE];
    int result[MATRIX_SIZE][MATRIX_SIZE];

    socklen_t length;

    if ((server_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_address,
             sizeof(server_address)) == -1) {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started and listening on port %d...\n", PORT);

    while (1) {
        length = sizeof(client_address);

        if (recvfrom(server_socket, buffer, MAX_BUFFER_SIZE, 0,
                     (struct sockaddr *)&client_address, &length) == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        memcpy(matrixA, buffer, sizeof(matrixA));

        if (recvfrom(server_socket, buffer, MAX_BUFFER_SIZE, 0,
                     (struct sockaddr *)&client_address, &length) == -1) {
            perror("Receive failed");
            exit(EXIT_FAILURE);
        }

        memcpy(matrixB, buffer, sizeof(matrixB));

        matrix_multiply(matrixA, matrixB, result);

        memcpy(buffer, result, sizeof(result));

        if (sendto(server_socket, buffer, sizeof(result), 0,
                   (struct sockaddr *)&client_address, length) == -1) {
            perror("Send failed");
            exit(EXIT_FAILURE);
        }
    }

    close(server_socket);
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
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[MAX_BUFFER_SIZE];

    int matrixA[MATRIX_SIZE][MATRIX_SIZE];
    int matrixB[MATRIX_SIZE][MATRIX_SIZE];
    int result[MATRIX_SIZE][MATRIX_SIZE];

    socklen_t length;

    if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
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
    sendto(client_socket, buffer, sizeof(matrixA), 0,
           (struct sockaddr *)&server_address, sizeof(server_address));

    memcpy(buffer, matrixB, sizeof(matrixB));
    sendto(client_socket, buffer, sizeof(matrixB), 0,
           (struct sockaddr *)&server_address, sizeof(server_address));

    length = sizeof(server_address);

    recvfrom(client_socket, buffer, MAX_BUFFER_SIZE, 0,
             (struct sockaddr *)&server_address, &length);

    memcpy(result, buffer, sizeof(result));

    printf("\nMatrix A:\n");
    print_matrix(matrixA);

    printf("\nMatrix B:\n");
    print_matrix(matrixB);

    printf("\nResult:\n");
    print_matrix(result);

    close(client_socket);
    return 0;
}
