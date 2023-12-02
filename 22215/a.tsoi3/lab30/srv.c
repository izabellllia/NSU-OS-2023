#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SOCKET_PATH "socket.sock"

int main(int argc, char *argv[]) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Couldn't create Socket!\n");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    unlink(SOCKET_PATH);
    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Binding error!\n");
        return -1;
    }

    if (listen(fd, 1) == -1) {
        perror("Error while listening!\n");
        return -1;
    }

    int cl = accept(fd, NULL, NULL);
    if (cl == -1) {
        perror("Couldn't accept client. Error!\n");
        close(fd);
        return -1;
    }

    char buf[100];
    int rc = read(cl, buf, sizeof(buf));
    while (rc > 0) {
        for(int i = 0; i < rc; i++){
            putchar(toupper(buf[i]));
        }
        rc = read(cl, buf, sizeof(buf));
    }
    if (rc == -1) {
        perror("Reading error!\n");
        return -1;
    }
    if (rc == 0) {
        close(cl);
        close(fd);
        return 0;
    }
}
