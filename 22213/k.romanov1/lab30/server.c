#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

#define SOCKET_NAME "server.sock" 
#define BUF_LEN 100

int main() {
    char buf[BUF_LEN]; 
    int fileDescriptor, clientDescriptor, returnValue;
    struct sockaddr_un addr;

    unlink(SOCKET_NAME);

    if ((fileDescriptor = socket(AF_UNIX, SOCK_STREAM, 0)) ==  -1) {
        perror("failed to create socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path)-1);

    if (bind(fileDescriptor, (struct sockaddr*) &addr, sizeof(addr))  == -1) {
        perror("failed to bind");
        exit(1);
    }

    if (listen(fileDescriptor, 5) == -1) {
        perror("failed to listen");
        exit(1);
    }

    while (1) {
        if ((clientDescriptor = accept(fileDescriptor, NULL, NULL)) == -1) {
            perror("failed to accept");
            continue;
        }

        while ((returnValue = read(clientDescriptor, buf, sizeof(buf))) > 0) {
            for (int i = 0; i < returnValue; i++) {
                buf[i] = (char) toupper(buf[i]);
                printf("%c", buf[i]);
            }
        }

        if (returnValue == -1) {
            perror("failed to read");
            exit(1);
        } else if (returnValue == 0) {
            printf("close connection\n");
            close(clientDescriptor);
            close(fileDescriptor);
            unlink(SOCKET_NAME);
            exit(0);
        }
    }
}
