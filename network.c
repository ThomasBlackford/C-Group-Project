#include <stdio.h>     //Input/output library - Used for perror(), printf(), and fprintf()
#include <stdlib.h>    //Standard library - Used for exit(), malloc(), and free()
#include <string.h>    //String library - Used for memset(), strlen()
#include <unistd.h>    //POSIX operating system API - Used for close(), accept()
#include <arpa/inet.h> //Library for Internet addresses and conversions - Used for socket(), bind(), listen(), accept(), ntohs(), htons()

#define PORT 8080            // Defines the port number for the server
#define MAX_CLIENTS 10       // Defines the maximum number of clients the server can handle
#define MAX_MESSAGE_LEN 1024 // Defines the maximum length of a message that can be sent or received
typedef struct
{
  int socket_fd;
  struct sockaddr_in address;
} Client;

// A bunch of prototype functions below

int create_server_socket();                                   // For creating a server socket
void bind_socket(int server_socket_fd);                       // for binding the server socket
void listen_for_connections(int server_socket_fd);            // for 'listening' for client connections
int accept_connection(int server_socket_fd, Client *clients); // for accepting client connections
void handle_client(Client *client);                           // for handling communication with a client
void send_message(Client *client, const char *message);       // for sending a message to a client
void receive_message(Client *client);                         // for receiving a message from a client

// The main function

int main()
{
  int server_socket_fd;                // Server socket descripter
  Client clients[MAX_CLIENTS];         // Array to store information about connected clients
  memset(clients, 0, sizeof(clients)); // Initialize the clients array with zeros

  server_socket_fd = create_server_socket(); // Create the server socket
  bind_socket(server_socket_fd);             // Bind the server socket to a port
  listen_for_connections(server_socket_fd);  //'Listen' for client connections

  // Use a conditional loop with a permanently true condition
  while (1)
  {
    int client_index = accept_connection(server_socket_fd, clients); // Accept connection from a client
    if (client_index == -1)
    {
      continue; // Error accepting connection==continue waiting for new connections
    }

    // For simplicity as I do not have a full understanding, I'm handling communication within the main loop
    handle_client(&clients[client_index]); // communication with the connected client
  }

  close(server_socket_fd); // Close the  socket
  return 0;
}

int create_server_socket()
{
  int server_socket_fd = socket(AF_INET, SOCK_STREAM, 0); // Create a TCP socket
  if (server_socket_fd == -1)
  {
    perror("Socket creation failed"); // Print error message
    exit(EXIT_FAILURE);               // exit the program with failure status
  }
  return server_socket_fd; // Return the file descriptor of the server socket
}

void bind_socket(int server_socket_fd)
{
  struct sockaddr_in server_addr;           // Structure to store server address information
  server_addr.sin_family = AF_INET;         // set address to IPv4
  server_addr.sin_addr.s_addr = INADDR_ANY; // set IP address to any available interface on the host
  server_addr.sin_port = htons(PORT);       // set port number and convert to network byte order

  if (bind(server_socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
  {
    perror("Binding failed"); // Print an error message
    exit(EXIT_FAILURE);       // exit the program with failure status
  }
}

void listen_for_connections(int server_socket_fd)
{
  if (listen(server_socket_fd, 5) == -1)
  {
    perror("Listening failed"); // Print an error message
    exit(EXIT_FAILURE);         // Exit the program with failure status
  }
}

int accept_connection(int server_socket_fd, Client *clients)
{
  int client_socket_fd;                       // File descriptor for the client socket
  struct sockaddr_in client_addr;             // structure to store client address information
  socklen_t client_len = sizeof(client_addr); // Length of client address structure

  client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&client_addr, &client_len); // Accept connection from a client
  if (client_socket_fd == -1)
  {
    perror("Acceptance failed"); // Print an error message
    return -1;                   // Return -1 to indicate error in accepting connection
  }

  int i;
  for (i = 0; i < MAX_CLIENTS; i++)
  {
    if (clients[i].socket_fd == 0)
    {
      clients[i].socket_fd = client_socket_fd; // Store  client socket file descriptor
      clients[i].address = client_addr;        // Store client address
      break;
    }
  }

  if (i == MAX_CLIENTS)
  {
    printf("Too many clients. Connection rejected.\n"); // Print a message indicating connection rejection
    close(client_socket_fd);                            // Close the client socket
    return -1;                                          // Return -1 to indicate connection rejection
  }

  return i; // Return the index of the client in the clients array
}

void handle_client(Client *client)
{
  while (1)
  {
    receive_message(client); // Receive message from the client
    // Process received message

    // Send response to client
    send_message(client, "Response message"); // Send a response message to the client
  }
}

void send_message(Client *client, const char *message)
{
  send(client->socket_fd, message, strlen(message), 0); // Send message to the client
}

void receive_message(Client *client)
{
  char buffer[MAX_MESSAGE_LEN];                                            // Buffer to store received message
  int bytes_received = recv(client->socket_fd, buffer, sizeof(buffer), 0); // Receive message from client
  if (bytes_received <= 0)
  {
    // Connection closed or error occurred
    close(client->socket_fd); // Close the client socket
    client->socket_fd = 0;    // Reset the client socket file descriptor
  }
  else
  {
    // Process received message
    buffer[bytes_received] = '\0';                        // Null-terminate the received message
    printf("Received message from client: %s\n", buffer); // Print the received message
  }
}
