#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./socket"
#define SIZE 256

int main(int argc, char *argv[]) {
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creating error");
        return -1;
    }

    struct sockaddr_un server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Binding error");
        return -1;
    }

    if (listen(server_socket, 1) == -1) {
        perror("Listening error");
        return -1;
    }

    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket == -1) {
        perror("Accept error");
        close(server_socket);
        return -1;
    }

    char buffer[SIZE];
    ssize_t bytesRead;
    while ((bytesRead = read(client_socket, buffer, sizeof(buffer))) > 0) {
        for(int i = 0; i < bytesRead; i++){
            buffer[i] = toupper(buffer[i]);
        }
        buffer[bytesRead - 1] = 0;
        printf("%s\n", buffer);
    }
    if (bytesRead == -1) {
        perror("Reading error");
        return -1;
    }

    unlink(SOCKET_PATH);
    close(client_socket);
    close(server_socket);
    printf("Closing server\n");
    return 0;
    
}