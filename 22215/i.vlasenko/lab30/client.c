#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./socket"
#define SIZE 256

int main() {
    int client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creating error");
        exit(-1);
    }

    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Binding error");
        close(client_socket);
        exit(-1);
    }

    char text[SIZE];
    while(1) {
        printf("> ");

        if (fgets(text, SIZE, stdin) == NULL){
            perror("Cannot read");
            exit(-1);
        }

        if (strlen(text) == 1) {
            break;
        }

        if (write(client_socket, text, strlen(text)) < 0){
            perror("Writing in socket error");
            exit(-1);
        }
    };

    close(client_socket);

    printf("Closing client\n");

    return 0;
}
