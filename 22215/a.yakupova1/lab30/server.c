#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define SOCKET_FILE "mysocket.sock"

int main(int argc, char *argv[]) {
    unlink(SOCKET_FILE);

    int fileDesc = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fileDesc == -1) {
        perror("error in socket");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_FILE);

    if (bind(fileDesc, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("error in bind");
        close(fileDesc);
        return -1;
    }

    if (listen(fileDesc, 1) == -1) {
        perror("error in listen");
        close(fileDesc);
        return -1;
    }

    int client = accept(fileDesc, NULL, NULL);
    if (client == -1) {
        perror("error in accept");
        close(fileDesc);
        return -1;
    }

    char buffer[1024];
    int chars;

    while ((chars=read(client, buffer, sizeof(buffer))) > 0) {
        for(int i = 0; i < chars; i++){
            putchar(toupper(buffer[i]));
        }
    }
    if (chars == -1) {
        perror("error in read");
        close(fileDesc);
        close(client);
        return -1;
    }
    if (chars == 0) {
	printf("connection closed\n");
        close(client);
        close(fileDesc);
	unlink(SOCKET_FILE);
        return 0;
    }
}
