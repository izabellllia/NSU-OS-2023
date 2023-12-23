#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <poll.h>

#define POLL_LENGTH (3)

int sock = -1;
char *socketPath = "./socket";

void int_sig() {
    if (sock != -1) {
        close(sock);
        unlink(socketPath);
    }
    write(1, "\ntranslation finished\n", 22);
    exit(1);
}

int add_new(struct pollfd *list, int fd) {
    int flag = -1;
    for (int i = 1; i < POLL_LENGTH; i++) {
        if (list[i].fd < 0) {
            list[i].fd = fd;
            list[i].events = POLLIN;
            flag = 1;
            break;
        }
    }
    return flag;
}

int main() {
    struct sockaddr_un socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sun_family = AF_UNIX;
    strcpy(socketAddr.sun_path, socketPath);

    signal(SIGINT, int_sig);

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (bind(sock, (struct sockaddr *) &socketAddr, sizeof(socketAddr)) == -1) {
        perror("bind");
        close(sock);
        exit(1);
    }

    if (listen(sock, 2) == -1) {
        perror("listen");
        close(sock);
        unlink(socketAddr.sun_path);
        exit(1);
    }

    struct pollfd poll_fds[POLL_LENGTH];
    for (int i = 0; i < POLL_LENGTH; i++) {
        poll_fds[i].fd = -1;
        poll_fds[i].events = POLLIN;
    }

    poll_fds[0].fd = sock;
    while (1) {
        if (poll(poll_fds, POLL_LENGTH, -1) == -1) {
            perror("poll failure");
            close(sock);
            unlink(socketAddr.sun_path);
            exit(1);
        }

        for (int i = 0; i < POLL_LENGTH; i++) {
            if (poll_fds[i].fd < 0) {
                continue;
            }
            if ((poll_fds[i].revents & POLLERR) || (poll_fds[i].revents & POLLHUP) ||
                (poll_fds[i].revents & POLLNVAL)) {
                close(poll_fds[i].fd);
                poll_fds[i].fd = -1;
                if (i == 0) {
                    printf("server failure");
                    close(sock);
                    unlink(socketAddr.sun_path);
                    exit(1);
                } else {
                    printf("close client\n");
                }
            }
        }

        int connectionDesc;
        if (poll_fds[0].revents & POLLIN) {
            if ((connectionDesc = accept(sock, NULL, NULL)) == -1) {
                perror("accept failure");
                close(sock);
                unlink(socketAddr.sun_path);
                exit(1);
            }
            if (add_new(poll_fds, connectionDesc) == -1) {
                perror("failed to add new connection");
            }
        }

        size_t readCount;
        char buffer[BUFSIZ];
        for (int i = 1; i < POLL_LENGTH; i++) {
            if (poll_fds[i].fd < 0) {
                continue;
            }
            if (poll_fds[i].revents & POLLIN) {
                if ((readCount = read(poll_fds[i].fd, buffer, BUFSIZ)) > 0) {
                    for (int j = 0; j < readCount; j++) {
                        buffer[j] = toupper(buffer[j]);
                    }
                    buffer[readCount] = 0;
                    printf("%s", buffer);
                }
                if (readCount == -1) {
                    perror("read");
                    close(sock);
                    unlink(socketAddr.sun_path);
                    exit(1);
                } else if (readCount == 0) {
                    printf("EOF: closing connection\n");
                    close(poll_fds[i].fd);
                    poll_fds[i].fd = -1;
                }
            }
        }
    }
}
