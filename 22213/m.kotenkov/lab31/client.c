#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#define SERVER_SOCK_FILE "server.sock"
#define MSGSIZE 20

int fd = -1;

void pipe_sig_handler();
void int_sig_handler();

int main() {
    struct sockaddr_un addr;
    char msgout[MSGSIZE];
    signal(SIGPIPE, pipe_sig_handler);
    signal(SIGINT, int_sig_handler);

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SERVER_SOCK_FILE);
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Connection failure");
        exit(EXIT_FAILURE);
    }

    int red = 0;
    while ((red = read(0, msgout, MSGSIZE)) > 0) {
        size_t to_send = red < MSGSIZE ? red : MSGSIZE;
        write(fd, msgout, to_send);
    };

    close(fd);
    exit(EXIT_SUCCESS);
}

void pipe_sig_handler() {
    if (fd != -1) {
        close(fd);
        write(2, "Writing to the socket failure\n", 31);
    }

    exit(EXIT_FAILURE);
}

void int_sig_handler() {
    if (fd != -1) {
        write(fd, "", 0);
        close(fd);
    }
    write(1, "\nTranslation finished\n", 22);

    exit(EXIT_SUCCESS);
}
