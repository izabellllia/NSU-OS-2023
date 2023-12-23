#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <ctype.h>

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
    write(1, "socket created\n", 15);

    atexit(close_and_unlink);

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, socket_path);


    if (bind(sockfd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        perror("bind error");
        exit(1);
    }
    write(1, "bind successful\n", 16);

    if (listen(sockfd, 0) == -1) {
        perror("listen error");
        exit(1);
    }
    write(1, "listen successful\n", 18);

    int client_fd;
    if ((client_fd = accept(sockfd, NULL, NULL)) == -1) {
        perror("accept error");
        exit(1);
    }

    char buf[BUFSIZ];
    ssize_t bytes_read;

    while (1) {
        while ((bytes_read = read(client_fd, buf, BUFSIZ)) > 0) {
            for (int i = 0; i < bytes_read; i++) {
                buf[i] = toupper(buf[i]);
            }
            write(1, buf, bytes_read);
        }

        if (bytes_read == 0) {
            printf("closing connection\n");
            close(client_fd);
            break;
        }
    }

    return 0;
}