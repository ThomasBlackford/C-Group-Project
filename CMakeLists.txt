cmake_minimum_required(VERSION 3.27)
project(untitled2 C)

set(CMAKE_C_STANDARD 11)

# Add an executable for the server
add_executable(server server.c)
# Link against the Winsock library
target_link_libraries(server Ws2_32)

# Add an executable for the client
add_executable(client client.c)
# Link against the Winsock library
target_link_libraries(client Ws2_32)
