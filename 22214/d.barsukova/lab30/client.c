#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

int sock = -1;

void pipe_sig() {
    if (sock != -1) {
        close(sock);
        write(2, "writing to the socket failure\n", 31);
    }
    exit(EXIT_FAILURE);
}

void int_sig() {
    if (sock != -1) {
        close(sock);
    }
    write(1, "\ntranslation finished\n", 22);
    exit(EXIT_SUCCESS);
}

int main() {
    struct sockaddr_un socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sun_family = AF_UNIX;
    char *socketPath = "./socket";
    strcpy(socketAddr.sun_path, socketPath);

    signal(SIGPIPE, pipe_sig);
    signal(SIGINT, int_sig);

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    if (connect(sock, (struct sockaddr *) &socketAddr, sizeof(socketAddr)) == -1) {
        perror("connection failure");
        close(sock);
        return -1;
    }

    size_t readCount = 0;
    char buffer[BUFSIZ];
    while (1) {
        readCount = read(0, buffer, BUFSIZ);
        write(sock, buffer, readCount);
    }

    close(sock);
    return 0;
}
