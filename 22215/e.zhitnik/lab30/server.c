#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#define BUFF_SIZE 256

const char *SOCKET_PATH = "mySocket";
int server_fd;

void normalExit() {
    exit(EXIT_SUCCESS);
}

void finishSocketWork() {
    if (access(SOCKET_PATH, F_OK) == 0) {
        unlink(SOCKET_PATH);
    }
    close(server_fd);
}

int main(int argc, char *argv[])
{
    int client_fd;
    struct sockaddr_un addr;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind error");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT, normalExit);
    atexit(finishSocketWork);

    if (listen(server_fd, 5) == -1) {
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        perror("accept error");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    char messageRd[BUFF_SIZE] = "";
    int len = 0;
    while ((len = read(client_fd, messageRd, BUFF_SIZE)) > 0) {
        for (int i = 0; i < len; i++) {
            if (putchar(toupper(messageRd[i])) == EOF) {
                perror("error while printing");
                close(client_fd);
                exit(EXIT_FAILURE);
            }
        }
    }

    if (len == -1) {
        perror("error while reading");
        close(client_fd);
        exit(EXIT_FAILURE);
    } else if (len == 0) {
        close(client_fd);
        return 0;
    }

    return 0;
}
