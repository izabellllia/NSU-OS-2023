#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>

#define SOCKET_NAME "server.sock"
#define BUFFER_SIZE 30

int main(int argc, char *argv[]) {
    int descriptor;
    if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Cannot create the socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX; // socket type
    strcpy(addr.sun_path, SOCKET_NAME); // socket file
    unlink(SOCKET_NAME);

    if (bind(descriptor, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Binding failure");
        exit(EXIT_FAILURE);
    }
    write(STDOUT_FILENO, "Socket created\n", 15);

    if (listen(descriptor, 1) == -1) {
        perror("Listening failure");
    }
    write(STDOUT_FILENO, "Server started listening\n", 25);

    int accepted;
    if ((accepted = accept(descriptor, NULL, NULL)) == -1) {
        perror("Accept failed");
    }
    write(STDOUT_FILENO, "Server accepted a connection\n", 29);

    long actuallyReadBytes;
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', BUFFER_SIZE);
    while ((actuallyReadBytes = read(accepted, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < actuallyReadBytes; i++) {
            buffer[i] = (char) toupper(buffer[i]);
        }

        if ((write(1, buffer, actuallyReadBytes)) == -1) {
            perror("Write to buffer error!");
            exit(EXIT_FAILURE);
        }
    }

    if (actuallyReadBytes == -1) {
        perror("Read failure");
        exit(EXIT_FAILURE);
    }
    write(STDOUT_FILENO, "Server finished its' work\n", 26);

    close(accepted);
    close(descriptor);
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}