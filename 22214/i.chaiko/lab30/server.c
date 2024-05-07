#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#define BUFFER_SIZE 2047

char socket_path[] = "just_socket";

void handler(int sign) {
    unlink(socket_path);
    exit(0);
}


int main(int argc, char *argv[]) {
    sigset(SIGINT, handler);
        struct sockaddr_un addr;
        int server_descriptor, client_descriptor;
        ssize_t rc;

        server_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);
        if (server_descriptor == -1) {
                perror("socket error");
                return - 1;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);


        if (bind(server_descriptor,(struct sockaddr*)&addr, sizeof(addr)) == -1) {
                perror("bind error");
                return -1;
        }

        if (listen(server_descriptor, 1) == -1) {
                unlink(socket_path);
		close(server_descriptor);
                perror("listen error");
                return -1;
        }

        if ((client_descriptor = accept(server_descriptor, NULL, NULL)) == -1) {
                unlink(socket_path);
		close(server_descriptor);
                perror("accept error");
                return -1;
        }

        char buffer[BUFFER_SIZE];
        while( (rc = read(client_descriptor, buffer, sizeof(buffer))) > 0) {
                for (int i = 0; i < rc; i++) {
            char c = toupper(buffer[i]);
            write(STDIN_FILENO, &c, 1);
                }
        }

        unlink(socket_path);

        if (rc == -1) {
                perror("read");
                return -1;
        } else if (rc == 0) {
                return 0;
        }

    return 0;
}
