#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

#define SERVER_NAME "server.sock"
#define BUF_LEN          100

int main() {
    char buf[BUF_LEN];
    int fileDescriptor, returnValue;
    struct sockaddr_un addr;

    if ((fileDescriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("failed to create socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SERVER_NAME, sizeof(addr.sun_path)-1);

    if (connect(fileDescriptor, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        close(fileDescriptor);
        perror("failed to connect");
        exit(1);
    }

    while ((returnValue = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if ((write(fileDescriptor, buf, returnValue) != returnValue)) {
            if (returnValue > 0) {
                perror("partial write");
            } else {
                close(fileDescriptor);
                perror("failed to write");
                exit(1);
            }
        }
    }

    close(fileDescriptor);
    exit(0);
}

