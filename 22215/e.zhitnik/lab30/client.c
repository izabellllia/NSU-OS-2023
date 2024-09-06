#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#define BUFF_SIZE 32
char *socket_path = "mySocket";

int main(int argc, char *argv[]){
    int client_fd;
    struct sockaddr_un addr;
    char buff[100];
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("socket errror");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    if (connect(client_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect error");
        exit(-1);
      }
    char messageWr[BUFF_SIZE] = "SuPeR sTrInG";
    write(client_fd, messageWr, BUFF_SIZE);
    close(client_fd);
}

