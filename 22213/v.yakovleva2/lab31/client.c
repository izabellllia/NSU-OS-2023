#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <errno.h>
#include <sys/time.h>

#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "server.sock"

int main() {
    char buf[100];
    int fd, rc;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket creating error");
        exit(1);
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SERVER_SOCK_FILE);

    // Implementing a timeout for the connection attempt
    struct timeval tv;
    tv.tv_sec = 10; // Timeout set to 10 seconds

    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) { //attempts to establish a connection with the server
        //EINPROGRESS indicates that the connection request has been initiated but hasn’t completed yet.
        if (errno == EINPROGRESS) {
            fd_set write_fds;
            FD_ZERO(&write_fds);
            FD_SET(fd, &write_fds);

            //to monitor the socket file descriptor fd for write readiness within a specified time
            //select requires the highest-numbered file descriptor in the set plus 1
            int result = select(fd + 1, NULL, &write_fds, NULL, &tv);
            int val;
            switch (result) {
                case -1:
                    perror("select error");
                    exit(1);
                    break;
                case 0:
                    fprintf(stderr, "Connection attempt timed out\n");
                    exit(1);
                    break;
                default:{
                    socklen_t len = sizeof(int);
                    //to get any error that might have occurred during the connection process
                    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &val, &len) == -1) {
                        perror("getsockopt error");
                        exit(1);
                    }
                    if (val != 0) {
                        fprintf(stderr, "Connection error\n");
                        exit(1);
                    }
                    break;
                }
            }

        } else {
            fprintf(stderr, "Connection failed\n");
            exit(1);
        }
    }

    while ((rc = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if ((write(fd, buf, rc) != rc)) {
            if (rc > 0) {
                perror("partial write");
            } else {
                perror("write error");
                exit(1);
            }
        }
    }

    close(fd);
    return 0;
}
