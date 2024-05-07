#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <poll.h>

#define MAX_CLIENTS 5
#define POLL_LENGTH (MAX_CLIENTS + 1)

int fd;

void close_and_unlink() {
    close(fd);
    unlink("./socket");
}

void sigCatch(int sig) {
    close_and_unlink();
}

int main() {
    char str[BUFSIZ];
    int saddrlen, cl, ret;
    struct sockaddr_un serv_addr;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Error with socket");
        exit(1);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, "./socket");
    saddrlen = sizeof(serv_addr);

    if (bind(fd, (struct sockaddr *) &serv_addr, saddrlen) == -1) {
        close(fd);
        perror("Error with bind");
        exit(1);
    }

    signal(SIGINT, sigCatch);

    atexit(close_and_unlink);

    if (listen(fd, MAX_CLIENTS) == -1) {
        perror("Error with listen");
        exit(1);
    }

    struct pollfd poll_fds[POLL_LENGTH];
    for (int i = 0; i < POLL_LENGTH; i++) {
        poll_fds[i].fd = -1;
        poll_fds[i].events = POLLIN;
    }

    poll_fds[0].fd = fd;

    int amount_of_conn = 0;

    while (1) {
        ret = poll(poll_fds, POLL_LENGTH, -1);

        if (ret == -1) {
            perror("Error with poll");
            exit(1);
        }

        for (int i = 0; i < POLL_LENGTH; i++) {
            if (poll_fds[i].fd < 0)
                continue;

            short revents = poll_fds[i].revents;
            if ((revents & POLLERR) || (revents & POLLHUP) || (revents & POLLNVAL)) {
                close(poll_fds[i].fd);
                poll_fds[i].fd = -1;

                if (i == 0) {
                    printf("Server error\n");
                    exit(1);
                } else {
                    printf("Socket closed\n");
                    amount_of_conn--;
                }
            }
        }

        if ((poll_fds[0].revents & POLLIN) && (amount_of_conn < POLL_LENGTH)) {
            if ((cl = accept(fd, NULL, NULL)) == -1) {
                perror("Error with accept");
                exit(1);
            }

            for (int i = 0; i < POLL_LENGTH; i++) {
                if (poll_fds[i].fd < 0) {
                    poll_fds[i].fd = cl;
                    break;
                }
            }
            amount_of_conn++;
            printf("New connection\n");
        }

        for (int i = 1; i < POLL_LENGTH; i++) {
            if (poll_fds[i].fd < 0) {
                continue;
            }

            if (poll_fds[i].revents & POLLIN) {
                ret = read(poll_fds[i].fd, str, BUFSIZ);
                if (ret > 0) {
                    for (int i = 0; i < ret; i++) {
                        str[i] = toupper(str[i]);
                    }
                    str[ret] = '\0';
                    printf("%s", str);
                }
                if (ret == -1) {
                    perror("Error with read");
                    exit(1);
                }
                if (ret == 0) {
                    printf("EOF reached, closing connection\n");
                    close(poll_fds[i].fd);
                    poll_fds[i].fd = -1;
                    amount_of_conn--;
                }
            }
        }
    }
    return 0;
}
