#include <winsock2.h>
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib") // Link with Ws2_32.lib

int main() {
    WSADATA wsaData;
    SOCKET s;
    struct sockaddr_in server;
    char message[2000], server_reply[2000];
    char server_ip[16]; // Buffer to store the user-provided IP address

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    // Create a socket
    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return 1;
    }

    // Prompt the user for the server IP address
    printf("Enter the server IP address: ");
    fgets(server_ip, sizeof(server_ip), stdin);
    server_ip[strcspn(server_ip, "\n")] = '\0'; // Remove the newline character

    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(8888);

    // Connect to the remote server
    if (connect(s, (struct sockaddr*)&server, sizeof(server)) < 0) {
        puts("connect error");
        return 1;
    }

    puts("Connected");

    // Keep communicating with the server
    while (1) {
        printf("Enter message (or 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = '\0'; // Remove the newline character

        // Check if the user wants to exit
        if (strcmp(message, "exit") == 0) {
            break;
        }

        // Send the message to the server
        if (send(s, message, strlen(message), 0) < 0) {
            puts("Send failed");
            return 1;
        }

        // Receive the reply from the server
        if (recv(s, server_reply, 2000, 0) < 0) {
            puts("recv failed");
            break;
        }

        printf("Server reply (%s): %s\n", server_ip, server_reply);
    }

    closesocket(s);
    WSACleanup();

    return 0;
}