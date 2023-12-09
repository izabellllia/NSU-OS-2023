#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

int main() {
    char str[BUFSIZ];
    int fd, saddrlen, cl, ret;
    struct sockaddr_un serv_addr;
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Error with socket");
        exit(1);
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sun_family = AF_UNIX;
    strcpy(serv_addr.sun_path, "./socket");
    saddrlen = sizeof(serv_addr);
    if (bind(fd, (struct sockaddr *) &serv_addr, saddrlen) == -1) {
        close(fd);
        perror("Error with bind");
        exit(1);
    }
    if (listen(fd, 0) == -1) {
        close(fd);
        unlink("./socket");
        perror("Error with listen");
        exit(1);
    }
    while (1) {
        cl = accept(fd, NULL, NULL);
        if (cl == -1) {
            perror("Error with accept");
            continue;
        }
        ret = read(cl, str, BUFSIZ);
        while (ret > 0) {
            for (int i = 0; i < ret; i++) {
                printf("%c", toupper(str[i]));
            }
            ret = read(cl, str, BUFSIZ);
        }
        if (ret == -1) {
            close(fd);
            close(cl);
            unlink("./socket");
            perror("Error with read");
            exit(1);
        }
        if (ret == 0) {
            close(cl);
            close(fd);
            unlink("./socket");
            exit(0);
        }
    }
    return 0;
}
