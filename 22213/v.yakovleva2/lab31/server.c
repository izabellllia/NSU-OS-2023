#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <poll.h>

#define BACKLOG (15)
#define POLL_LENGTH (BACKLOG + 1)

#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "server.sock"

int newConnection(struct pollfd *poll_list, int fd);

int main() {

    unlink(SERVER_SOCK_FILE); //if socket is  already in use

    char buf[100]; //to store data read from the socket
    int fd, cl, rc; //fd for the server socket descriptor,
    //cl for the client socket descriptor,
    //and rc for the return code of various system calls.

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) ==  -1) {
        perror("socket creating error");
        exit(1);
    }

    struct sockaddr_un addr; //to specify the address of the socket
    memset(&addr, 0, sizeof(addr));//clear the structure
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SERVER_SOCK_FILE); //server socket's path (file path) is specified

    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr))  == -1) {
    //This code binds the server socket to a specific address
        perror("bind error");
        exit(1);
    }

    //prepares the server socket to accept incoming connections. The listen function is used,
    //and it specifies the maximum number of pending connections (backlog) as 5
    if (listen(fd, BACKLOG) == -1) {
        perror("listen error");
        exit(1);
    }

    struct pollfd poll_fds[POLL_LENGTH];
    for (int i = 0; i < POLL_LENGTH; i++) {
        poll_fds[i].fd = -1;
        poll_fds[i].events = POLLIN | POLLPRI;

    }

    poll_fds[0].fd = fd;

    while (1) {
        if ((poll(poll_fds, POLL_LENGTH, -1)) == -1) {
            perror("bad poll");
            exit(1);
        }

        for (int i = 0; i < POLL_LENGTH; i++) {
            if (poll_fds[i].fd < 0){
                short revents = poll_fds[i].revents;

                if ((revents & POLLERR) || (revents & POLLHUP) || (revents & POLLNVAL)) {
                    close(poll_fds[i].fd);
                    poll_fds[i].fd = -1;

                    if (i == 0) {
                        printf("server error");
                        exit(1);
                    } else {
                        printf("closing socket\n");
                    }
                }
            }
        }

        if ((poll_fds[0].revents & POLLIN) || (poll_fds[0].revents & POLLPRI)) {
            if ((cl = accept(fd, NULL, NULL)) == -1) {
                perror("accept error");
                continue;
            }

            if (newConnection(poll_fds, cl) == -1) {
                perror("add new connection fail");
            }
        }

        for (int i = 1; i < POLL_LENGTH; i++) {
            if (poll_fds[i].fd < 0) continue;
            int cur_desc = poll_fds[i].fd;

            if ((poll_fds[i].revents & POLLIN) || (poll_fds[i].revents & POLLPRI)) {
                if ((rc = read(cur_desc, buf, sizeof(buf))) > 0) {
                    for (int j = 0; j < rc; j++) {
                        buf[j] = toupper(buf[j]);
                        printf("%c", buf[j]);
                    }
                }

                if (rc == -1) {
                    perror("read");
                    exit(-1);
                } else if (rc == 0) {
                    printf("closing connection\n");
                    close(cur_desc);
                    poll_fds[i].fd = -1;
                }
            }
        }
    }
}

int newConnection(struct pollfd *poll_list, int fd) {
    int result = -1;

    for (int i = 1; i < POLL_LENGTH; i++) {
        if (poll_list[i].fd < 0) {
            result = 1;
            poll_list[i].fd = fd;
            poll_list[i].events = POLLIN | POLLPRI;

            result = 0;
            break;
        }
    }

    return result;
}
