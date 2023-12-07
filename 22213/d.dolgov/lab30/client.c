#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
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
        exit(EXIT_FAILURE);
    }
    write(STDOUT_FILENO, "Connected to the server\n", 24);

    while (1) {
        long red = read(STDIN_FILENO, msgout, BUFFER_SIZE);
        size_t dataLength = red < BUFFER_SIZE ? red : BUFFER_SIZE;
        write(descriptor, msgout, dataLength);
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