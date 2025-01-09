#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

char name[50];

void *receive_message(void *socket_desc) {
    int client_socket = *((int *)socket_desc);
    char buffer[BUFFER_SIZE];
    int bytes_received;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }

    return NULL;
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char message[BUFFER_SIZE];
    char formatted_message[BUFFER_SIZE + 50];
    pthread_t receive_thread;

    printf("Enter your name: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = '\0';

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to server. You can start chatting!\n");

    pthread_create(&receive_thread, NULL, receive_message, (void *)&client_socket);

    while (1) {
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';

        snprintf(formatted_message, sizeof(formatted_message), "%s - %s", name, message);

        if (send(client_socket, formatted_message, strlen(formatted_message), 0) < 0) {
            perror("Send failed");
        }
    }

    close(client_socket);
    return 0;
}
