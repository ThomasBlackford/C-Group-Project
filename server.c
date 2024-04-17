#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib") // Link with Ws2_32.lib

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in server, client;
    int c;
    char message[2000];
    struct hostent *host;
    char *ip_address;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    // Create a socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind
    if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        return 1;
    }

    // Get the IP address of the machine running the server
    host = gethostbyname("");
    if (host == NULL) {
        printf("Failed to get host name.\n");
        return 1;
    }
    ip_address = inet_ntoa(*(struct in_addr*)*host->h_addr_list);
    printf("Server is listening on: %s\n", ip_address);

    // Listen to incoming connections
    listen(serverSocket, 3);

    puts("Waiting for incoming connections...");

    while (1) {
        c = sizeof(struct sockaddr_in);
        clientSocket = accept(serverSocket, (struct sockaddr*)&client, &c);
        if (clientSocket == INVALID_SOCKET) {
            printf("Accept failed with error code : %d", WSAGetLastError());
            return 1;
        }

        printf("Connection accepted from %s\n", inet_ntoa(client.sin_addr));

        // Communicate with the client
        while (1) {
            memset(message, 0, sizeof(message)); // Clear the message buffer
            if (recv(clientSocket, message, sizeof(message), 0) < 0) {
                puts("Recv failed");
                break;
            }
            printf("Client (%s): %s\n", inet_ntoa(client.sin_addr), message);

            printf("Enter message: ");
            gets(message); // Using gets() for simplicity, but it's unsafe. Consider using fgets() in real applications.

            // Send the message to the client
            if (send(clientSocket, message, strlen(message), 0) < 0) {
                puts("Send failed");
                break;
            }
        }

        closesocket(clientSocket);
    }

    WSACleanup();

    return 0;
}