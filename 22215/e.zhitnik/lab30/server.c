#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

#define BUFF_SIZE 32
char *socket_path = "mySocket";

int main(int argc, char *argv[])
{
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buff[100];
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("socket errror");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;

    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    unlink(socket_path); 
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("bind error");
        exit(EXIT_FAILURE);
      }
    
    if (listen(server_fd, 5) == -1)
    {
        perror("listen error");
        exit(EXIT_FAILURE);
        
    }
    if ((client_fd = accept(server_fd, NULL, NULL)) == -1){
        perror("accept error");
    }

    char messageRd[BUFF_SIZE];
    int len;
    while((len = read(client_fd, messageRd, BUFF_SIZE)) > 0)
    {
        for (int i = 0; i < len; i++){
            putchar(toupper(messageRd[i]));
        }
    }
    if (len == -1) {
        perror("error while reading");
        exit(EXIT_FAILURE);
    }
    else if (len == 0){
        close(server_fd);
        close(client_fd);
        return 0;
    }
}

