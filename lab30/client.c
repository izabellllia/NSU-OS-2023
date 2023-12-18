#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
char *path = "./socket";
int main(int argc, char *argv[]) {
    struct sockaddr_un address;
    char buffer[100];
    int descriptor;
    if ( (descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("error in socket");
        return -1;
    }
    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, path, sizeof(address.sun_path)-1);
    if (connect(descriptor, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("error while connecting");
        return -1;
    }

    ssize_t charsRead, bytesWritten, currentWrite;

    do {
        charsRead = read(STDIN_FILENO, buffer, sizeof(buffer));
        if (charsRead == -1) {
            perror("Error while reading input");
            return -1;
        }

        bytesWritten = 0;
        currentWrite = 0;
        do {
            ssize_t remainingWrite = charsRead - bytesWritten;
            currentWrite = write(descriptor, buffer + bytesWritten, remainingWrite);
            if (currentWrite == -1) {
                perror("Error while writing output");
                return -1;
            }
            bytesWritten += currentWrite;
        } while (bytesWritten < charsRead);

    } while (charsRead > 0);


    close(descriptor);
    return 0;
}




