#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>

#define CLIENT_SOCK_FILE "client.sock"
#define SERVER_SOCK_FILE "server.sock"

int main() {
    
        unlink(SERVER_SOCK_FILE); //if socket is  already in use
    
        char buf[100]; //to store data read from the socket
        int fd, cl, rc; //fd for the server socket descriptor,
        //cl for the client socket descriptor,
        //and rc for the return code of various system calls.

        if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) ==  -1) {
                perror("socket creating error");
                exit(1);
        }

        struct sockaddr_un addr; //to specify the address of the socket
        memset(&addr, 0, sizeof(addr));//clear the structure
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, SERVER_SOCK_FILE); //server socket's path (file path) is specified

        if (bind(fd, (struct sockaddr *) &addr, sizeof(addr))  == -1) {
        //This code binds the server socket to a specific address
                perror("bind error");
                exit(1);
        }

        //prepares the server socket to accept incoming connections. The listen function is used,
        //and it specifies the maximum number of pending connections (backlog) as 5
        if (listen(fd, 5) == -1) {
                perror("listen error");
                exit(1);
        }

        while (1) {
        //accept incoming client connections. If successful,
        //it returns a new socket descriptor (cl) for the accepted connection.
        //If there's an error, it prints an error message and continues
        //to the next iteration of the loop.
                if ((cl=accept(fd, NULL, NULL)) == -1) {
                        perror("accept error");
                        continue;
                }
                //reads data from the client socket (cl) into the buf array.
                //The read function reads data from the socket,
                //and the loop continues as long as data is successfully read
                while ((rc = read(cl, buf, sizeof(buf))) > 0) {
                        for (int i = 0; i < rc; i++) {
                                buf[i] = (char) toupper(buf[i]);
                                printf("%c", buf[i]);
                        }
                }

                if (rc == -1) {
                        perror("read");
                        exit(1);
                }

                if (rc == 0) {
                        printf("closing connection\n");
                        close(cl);
                        close(fd);
                        unlink(SERVER_SOCK_FILE);
                        return 0;
                }
        }
}
