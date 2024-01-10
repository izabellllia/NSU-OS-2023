#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *socket_path = "./socket";

int main(int argc, char *argv[]) {
        struct sockaddr_un addr;
        char buf[100];
        int fd,cl,rc;

        if ( (fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
                perror("socket error");
                return -1;
        }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
        unlink(socket_path);

        if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
                perror("bind error");
                return -1;
        }

        if (listen(fd, 5) == -1) {
                perror("listen error");
                return -1;
        }

        if ( (cl = accept(fd, NULL, NULL)) == -1) {
                perror("accept error");
                close(fd);
                return -1;
        }

        while ( (rc=read(cl,buf,sizeof(buf))) > 0) {
                int i;
                for(i = 0; i < rc; i++){
                        putchar(toupper(buf[i]));
                }
        }
        if (rc == -1) {
                perror("read");
                return -1;
        }
        else if (rc == 0) {
                close(cl);
                close(fd);
                return 0;
        }
}
