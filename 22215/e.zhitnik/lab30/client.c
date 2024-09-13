#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define BUFF_SIZE 256

const char *SOCKET_PATH = "mySocket";

int main(int argc, char *argv[]){
    int client_fd;
    struct sockaddr_un addr;
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket errror");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);
    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect error");
        exit(-1);
    }
    char messageWr[BUFF_SIZE] = "";
    int len = 0;
    while ((len = read(STDIN_FILENO, messageWr, BUFF_SIZE-1)) > 0) {
        if (write(client_fd, messageWr, len) == -1) {
            perror("write error");
            break;
        }
    }
    if (len == -1) {
        perror("read error");
    }
    close(client_fd);
}

