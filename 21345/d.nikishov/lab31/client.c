#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char* argv[]) {
    int f_d;

    if (argc < 2) {
        f_d = STDIN_FILENO;
    }
    else if ((f_d = open(argv[1], O_RDONLY)) == -1) {
        perror("Cant open file");
        exit(1);
    }

    int socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Error while creating socket");
        return 1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    char* socket_path = "./socket";
    strcpy(addr.sun_path, socket_path);

    if (connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Error while connecting");
        close(socket_fd);
        return 1;
    }

    int read_count;
    char buf[BUFSIZ];

    while ((read_count = read(f_d, buf, BUFSIZ)) > 0) {
        if (write(socket_fd, buf, read_count) != read_count) {
            perror("Error while writing");
            close(socket_fd);
            return 1;
        }
    }

    if (f_d != STDIN_FILENO) close(f_d);
    close(socket_fd);
    return 0;
}
