//functions for setting up sockets, handling incoming connections, sending and receiving data over the network, etc.

#include <stdio.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_client(SOCKET client_socket)
{
  char buffer[BUFFER_SIZE];
  int bytes_received;

  // Receive data from the client
  while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0)
  {
    // Print received data
    buffer[bytes_received] = '\0'; // Null-terminate the received data
    printf("Received: %s\n", buffer);

    // Back to the client
    send(client_socket, buffer, strlen(buffer), 0);
  }

  // Close connection
  closesocket(client_socket);
}

int main()
{
  WSADATA wsa;
  SOCKET server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  int client_addr_len = sizeof(client_addr);

  // Initialize Winsock
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
  {
    printf("WSAStartup failed\n");
    return 1;
  }

  // Create socket
  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
  {
    printf("Socket creation failed\n");
    return 1;
  }

  // Set up server address struct
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  // Bind socket to address and port
  if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
  {
    printf("Bind failed\n");
    return 1;
  }

  // Listen for incoming connections
  if (listen(server_socket, 3) == SOCKET_ERROR)
  {
    printf("Listen failed\n");
    return 1;
  }

  printf("Server listening on port %d\n", PORT);

  // Accept incoming connections
  while (1)
  {
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == INVALID_SOCKET)
    {
      printf("Accept failed\n");
      return 1;
    }

    printf("New connection accepted\n");

    // Handle client in a separate function or thread
    handle_client(client_socket);
  }

  // Cleanup
  closesocket(server_socket);
  WSACleanup();

  return 0;
}
