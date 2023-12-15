#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

int main() {
    int sock;
    struct sockaddr_un socketAddr;
    memset(&socketAddr, 0, sizeof(socketAddr));
    socketAddr.sun_family = AF_UNIX;
    char *socketPath = "./socket";
    strcpy(socketAddr.sun_path, socketPath);

    if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }

    if ((bind(sock, (struct sockaddr *) &socketAddr, sizeof(socketAddr))) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    if (listen(sock, 1) != 0) {
        perror("listen");
        close(sock);
        unlink(socketAddr.sun_path);
        return -1;
    }

    int connectionDesc = accept(sock, NULL, NULL);
    if (connectionDesc == -1) {
        perror("accept failure");
        close(sock);
        unlink(socketAddr.sun_path);
        return -1;
    }

    size_t readCount;
    char buffer[BUFSIZ];
    while ((readCount = read(connectionDesc, buffer, BUFSIZ)) > 0) {
        for (int i = 0; i < readCount; i++) {
            buffer[i] = toupper(buffer[i]);
        }
        if ((write(1, buffer, readCount)) == -1) {
            perror("write failure");
            close(connectionDesc);
            close(sock);
            unlink(socketAddr.sun_path);
            return -1;
        }
    }

    if (readCount == -1) {
        perror("read failure");
        close(connectionDesc);
        close(sock);
        unlink(socketAddr.sun_path);
        return -1;
    }

    if (readCount == 0) {
        printf("connection was closed\n");
    }

    printf("\n");
    close(connectionDesc);
    close(sock);
    unlink(socketAddr.sun_path);
    return 0;
}
