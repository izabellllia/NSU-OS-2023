#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char socket_path[] = "just_socket";

int main() {
    struct sockaddr_un socket_address;
    int file_descriptor;
    size_t rc;
    char buffer[2047];

    file_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);
    if (file_descriptor == -1) {
        perror("client: failed to create socket\n");
        return -1;
    }

    memset(&socket_address, 0, sizeof(socket_address));
    socket_address.sun_family = AF_UNIX;
    strncpy(socket_address.sun_path, socket_path, sizeof(socket_address.sun_path) - 1);

    if (connect(file_descriptor, (struct sockaddr*)&socket_address, sizeof(socket_address)) == -1) {
        perror("connect error");
        return -1;
    }

    while ((rc = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
        write(file_descriptor, buffer, rc);
    }

    return 0;
}
