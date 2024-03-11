#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 4
#define BUFFER_SIZE 1024
#define PASSPHRASE "joe"

typedef struct {
    int client_socket;
    struct sockaddr_in client_address;
} client_info;

void *handle_client(void *arg);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    pthread_t thread_id;
    socklen_t client_address_len;
    char passphrase_attempt[100];

    // Create a server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(8080); // Example port number

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server started. Waiting for connections...\n");

    // Accept incoming connections and handle them in separate threads
    while (1) {
        // Accept a connection
        client_address_len = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len);
        if (client_socket == -1) {
            perror("Accept failed");
            continue;
        }

        // Authenticate the client with a passphrase
        // Replace this with your actual authentication logic
        printf("Enter the passphrase: ");
        scanf("%s", passphrase_attempt);
        if (strcmp(passphrase_attempt, PASSPHRASE) != 0) {
            printf("Incorrect passphrase. Closing connection.\n");
            close(client_socket);
            continue;
        }

        // Pass client socket to handle_client thread
        int *arg = malloc(sizeof(*arg));
        if (arg == NULL) {
            perror("Memory allocation failed");
            close(client_socket);
            continue;
        }
        *arg = client_socket;

        if (pthread_create(&thread_id, NULL, handle_client, arg) != 0) {
            perror("Thread creation failed");
            close(client_socket);
            free(arg);
            continue;
        }

        // Send "Mike connected" message to the client
        const char *message = "Mike connected";
        send(client_socket, message, strlen(message), 0);

        printf("Client connected: %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
