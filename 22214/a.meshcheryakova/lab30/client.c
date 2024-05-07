#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>

int main() {
    char str[BUFSIZ];
    int fd, ret;
    struct sockaddr_un cl_addr;
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Error with socket");
        exit(1);
    }
    memset(&cl_addr, 0, sizeof(cl_addr));
    cl_addr.sun_family = AF_UNIX;
    strcpy(cl_addr.sun_path, "./socket");
    ret = connect(fd, (struct sockaddr *) &cl_addr, sizeof(cl_addr));
    if (ret == -1) {
        close(fd);
        perror("Errror with connect");
        exit(1);
    }
    while(1) {
        if (fgets(str, BUFSIZ, stdin) == NULL) {
            close(fd);
            perror("Error with fgets");
            exit(1);
        }
        if (str[0] == '.' && str[1] == '\n') {
            break;
        }
        ret = write(fd, str, strlen(str));
        if (ret == -1) {
            close(fd);
            perror("Error with write");
            exit(1);
        }
 }
    close(fd);
    return 0;
}
