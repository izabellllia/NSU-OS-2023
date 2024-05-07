#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SOCKET_PATH "socket.sock"

int main(int argc, char *argv[]) {

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Socket creating error");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Connection error");
        return -1;
    }

    char buf[100];
    memset(buf, 0, sizeof(buf));
    int read_chars = 0;
    int written_bytes;
    int curr_write;
    int need_write;
    do {
        read_chars = read(STDIN_FILENO, buf, sizeof(buf));
        if (read_chars == -1) {
            perror("Error while reading line from user");
            return -1;
        }

        written_bytes = 0;
        curr_write = 0;
        do {
            need_write = read_chars - written_bytes;
            printf("Read: %d, Need: %d\n", read_chars, need_write);
            curr_write = write(fd, buf + written_bytes, need_write);
            if (curr_write == -1) {
                perror("ERROR WRITE ERROR");
                return -1;
            }
            written_bytes += curr_write;
            printf("Curr: %d, Left: %d\n", curr_write, written_bytes);
        } while (written_bytes < read_chars);

    } while (read_chars > 0);

    close(fd);
    return 0;
}
