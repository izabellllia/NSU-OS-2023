#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>

#define SOCKET_NAME "server.sock"
#define BUFFER_SIZE 30

int main() {
    int descriptor;
    if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Cannot create the socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_NAME);
    unlink(SOCKET_NAME);

    if (bind(descriptor, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("Binding failure");
        exit(EXIT_FAILURE);
    }
    printf("Socket created\n");

    if (listen(descriptor, 1) == -1) {
        close(descriptor);
        perror("Listening failure");
        exit(EXIT_FAILURE);
    }
    printf("Server started listening\n");

    int accepted;
    if ((accepted = accept(descriptor, NULL, NULL)) == -1) {
        perror("Accept failed");
        close(descriptor);
        exit(EXIT_FAILURE);
    }
    printf("Server accepted a connection\n");

    long actuallyReadBytes;
    char buffer[BUFFER_SIZE];
    while ((actuallyReadBytes = read(accepted, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < actuallyReadBytes; i++) {
            buffer[i] = (char) toupper(buffer[i]);
        }

        if ((write(1, buffer, actuallyReadBytes)) == -1) {
            perror("Write to buffer error!");
            close(accepted);
            close(descriptor);
            unlink(SOCKET_NAME);
            exit(EXIT_FAILURE);
        }
    }

    if (actuallyReadBytes == -1) {
        perror("Read failure");
        close(accepted);
        close(descriptor);
        unlink(SOCKET_NAME);
        exit(EXIT_FAILURE);
    }
    printf("Server finished its' work\n");

    close(accepted);
    close(descriptor);
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}