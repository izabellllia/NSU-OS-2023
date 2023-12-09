#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>

#define SERVER_NAME "server.sock"
#define BUF_LEN          100

int fileDescriptor = -1;

int main() {
    char buf[BUF_LEN];
    int returnValue;
    struct sockaddr_un addr;

    struct sigaction ign_sigpipe;
    ign_sigpipe.sa_handler = SIG_IGN;
    if (sigaction(SIGPIPE, &ign_sigpipe, NULL) == -1) {
        perror("faield to sigaction ign_sigpipe");
        exit(1);
    }

    if ((fileDescriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("failed to create socket");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SERVER_NAME, sizeof(addr.sun_path)-1);

    if (connect(fileDescriptor, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        close(fileDescriptor);
        perror("failed to connect");
        exit(1);
    }

    while ((returnValue = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if ((write(fileDescriptor, buf, returnValue) != returnValue)) {
            close(fileDescriptor);
            perror("Connection was closed");
            exit(1);
        }
    }

    close(fileDescriptor);
    exit(0);
}

