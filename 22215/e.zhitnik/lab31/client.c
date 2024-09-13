#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

char *path = "./socket";

int main()
{
    struct sockaddr_un address;
    char buff[BUFFER_SIZE];
    int descriptor;

    if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Can't create socket");
        return -1;
    }

    address.sun_family = AF_UNIX;
    memset(address.sun_path, 0, sizeof(address.sun_path));
    strncpy(address.sun_path, path, sizeof(address.sun_path) - 1);

    if (connect(descriptor, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Error while connecting");
        close(descriptor);
        return -1;
    }

    memset(buff, 0, BUFFER_SIZE);
    ssize_t count;
    while ((count = read(STDIN_FILENO, buff, BUFFER_SIZE)) > 0) {
        if (write(descriptor, buff, count) != count) {
            perror("Error while writing");
            close(descriptor);
            return -1;
        }
    }

    if (count < 0) {
        perror("Error while reading from stdin");
        close(descriptor);
        return -1;
    }

    close(descriptor);
    return 0;
}
