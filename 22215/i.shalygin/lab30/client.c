#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int main() {
    struct sockaddr_un addr;
    char message[BUFSIZ];
    int fd,cl,rc;
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if ((fd) == -1) {
        perror("socket error");
        return -1;
    }
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path,"socket",sizeof(addr.sun_path)-1); 
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect error");
        return -1;
    }

    printf("Connected\n");
    printf("Write your text:\n");
    while(1){
    if (fgets(message,BUFSIZ,stdin) == NULL){
            perror("Cannot read");
            return -1;
        }
        if (write(fd, message, strlen(message)) == -1){
            perror("Couldn't write in socket");
            return -1;
        }
    }
    
    return 0;
}