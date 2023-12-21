#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>

#define SOCK_FILE "server.sock"
#define BUFFER_SIZE 30

int descriptor = -1;

void sigPipeHandler();

void sigInterruptHandler();

int main() {
    struct sockaddr_un addr;
    char msgout[BUFFER_SIZE];
    signal(SIGPIPE, sigPipeHandler);
    signal(SIGINT, sigInterruptHandler);

    if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_FILE);

    if (connect(descriptor, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("Connection failure");
        close(descriptor);
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server\n");

    while (1) {
        long red;
        if ((red = read(STDIN_FILENO, msgout, BUFFER_SIZE)) == -1) {
            perror("Read from buffer error!");
            close(descriptor);
            exit(EXIT_FAILURE);
        }

        size_t dataLength = red < BUFFER_SIZE ? red : BUFFER_SIZE;

        if ((write(descriptor, msgout, dataLength)) == -1) {
            perror("Write to buffer error!");
            close(descriptor);
            exit(EXIT_FAILURE);
        }
    }
}

void sigPipeHandler() {
    if (descriptor != -1) {
        close(descriptor);
        write(STDERR_FILENO, "Error when writing to the socket!\n", 35);
    }

    exit(EXIT_FAILURE);
}

void sigInterruptHandler() {
    if (descriptor != -1) {
        close(descriptor);
    }
    write(STDOUT_FILENO, "\nClient finished its' work\n", 27);

    exit(EXIT_SUCCESS);
}