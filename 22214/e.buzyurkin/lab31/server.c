#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <poll.h>
#include <ctype.h>
#include <signal.h>

#define MY_MAXCONN 5

char *socket_path = "server_socket";
int sockfd;

void close_and_unlink() {
    close(sockfd);
    unlink(socket_path);
}

int main() {

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(1);
    }
    printf("socket created\n");

    atexit(close_and_unlink);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);

    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        perror("bind error");
        exit(1);
    }
    printf("bind successful\n");

    if (listen(sockfd, MY_MAXCONN) == -1) {
        perror("listen error");
        exit(1);
    }
    printf("listen successful\n");


    struct pollfd conn_fds[MY_MAXCONN];
    for (int i = 0; i < MY_MAXCONN; i++) {
        conn_fds[i].fd = -1;
        conn_fds[i].events = POLLIN;
    }
    conn_fds[0].fd = sockfd;

    int conn_count = 0;

    int client_fd;
    char buf[BUFSIZ + 1];
    ssize_t bytes_read;
    while (1) {
        if (poll(conn_fds, MY_MAXCONN, 0) == -1) {
            perror("poll error");
            exit(1);
        }

        for (int i = 0; i < MY_MAXCONN; i++) {
            if (conn_fds[i].fd < 0) 
                continue;

            if ((conn_fds[i].revents & POLLERR) 
                    || (conn_fds[i].revents & POLLHUP) 
                    || (conn_fds[i].revents & POLLNVAL)) 
                {

                close(conn_fds[i].fd);
                conn_fds[i].fd = -1;

                if (i == 0) {
                    printf("server error\n");
                    exit(1);
                } else {
                    printf("socket closed\n");
                    conn_count--;
                }
            }
        }


        if (conn_fds[0].revents & POLLIN && conn_count < MY_MAXCONN) {
            if ((client_fd = accept(sockfd, NULL, NULL)) == -1) {
                perror("accept error");
                exit(1);
            }

            for (int i = 0; i < MY_MAXCONN; i++) {
                if (conn_fds[i].fd < 0) {
                    conn_fds[i].fd = client_fd;
                    conn_fds[i].events = POLLIN;
                    break;
                }
            }
            
            conn_count++;
            printf("new connection\n");
        }

    
        for (int i = 1; i < MY_MAXCONN; i++) {
            if (conn_fds[i].fd < 0) {
                continue;
            }

            if (conn_fds[i].revents & POLLIN) {
                if ((bytes_read = read(conn_fds[i].fd, buf, BUFSIZ)) > 0) {
                    for (int j = 0; j < bytes_read; ++j) {
                        buf[j] = toupper(buf[j]);
                    }
                    buf[bytes_read] = 0;
                    printf("%s", buf);
                    fflush(stdout);
                }
                else if (bytes_read == 0) {
                    printf("EOF, closing connection\n");
                    close(conn_fds[i].fd);
                    conn_fds[i].fd = -1;
                }
            }
        }   
    }

}