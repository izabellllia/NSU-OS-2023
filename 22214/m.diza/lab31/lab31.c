#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

int fd = -1;

void sigpipe_handler();
void sigint_handler();
void finish();

int main() {
    atexit(finish);
    struct sockaddr_un addr;
    signal(SIGPIPE, sigpipe_handler);
    signal(SIGINT, sigint_handler);

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Socket error");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "mysocket", sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Connect error");
        exit(1);
    }

    char buffer[BUFSIZ];
    int len;

    while (1) {
        if ((len = read(0, buffer, BUFSIZ)) < 0) {
            perror("Read error");
            exit(1);
        }
        if (write(fd, buffer, len) == -1) {
            perror("Write error");
            exit(1);
        }
    };

    exit(0);
}

void sigpipe_handler() {
    write(2, "sigpipe\n", 8);
    exit(1);
}

void sigint_handler() {
    exit(0);
}

void finish() {
    if (fd != -1)
        close(fd);
}