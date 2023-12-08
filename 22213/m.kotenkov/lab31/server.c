#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>

#define SERVER_SOCK_FILE "server.sock"
#define MSGSIZE 20

int fd = -1;

void int_sig_handler();

int main() {
    int sd, len, status, new_socket, active, max_sd, client_socket[10], max_clients = 10;
    struct sockaddr_un addr;
    fd_set readfds;
    char msgin[MSGSIZE];
    memset(msgin, '\0', MSGSIZE);

    signal(SIGINT, int_sig_handler);
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failure");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SERVER_SOCK_FILE);

    unlink(SERVER_SOCK_FILE);
    if ((status = bind(fd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
        perror("Binding failure");
        exit(EXIT_FAILURE);
    }
    if (listen(fd, 2) == -1) {
        perror("Listening failure");
    }


    for (int i = 0; i < max_clients; i++) {   
        client_socket[i] = 0;   
    }

    while (1) { 
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        max_sd = fd;

        for (int i = 0 ; i < max_clients ; i++) {
            int sd = client_socket[i];
            
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        active = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((active < 0) && (errno != EINTR)) {
            perror("Select failure");
            exit(EXIT_FAILURE);
        }


        if (FD_ISSET(fd, &readfds)) {
            if ((new_socket = accept(fd, NULL, NULL)) == -1) {
                perror("Accept failure");
                exit(EXIT_FAILURE);
            }
            
            for (int i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets at index %d\n" , i);
                    break;
                }
            }
        }

        for (int i = 0; i < max_clients; i++) {
            sd = client_socket[i];
             
            if (FD_ISSET(sd, &readfds)) {
                if ((len = read(sd, msgin, MSGSIZE)) == 0) {
                    close(sd);
                    client_socket[i] = 0;
                } else {
                    for (int i = 0; i < len; i++) {
                        msgin[i] = toupper(msgin[i]);
                    }

                    if ((write(1, msgin, len)) == -1) {
                        perror("Write failure");
                        exit(EXIT_FAILURE);
                    }
                }
            }
        }
	}
}

void int_sig_handler() {
    if (fd != -1) {
        close(fd);
    }
    write(1, "\nReceiving finished\n", 21);

    unlink(SERVER_SOCK_FILE);
    exit(EXIT_SUCCESS);
}
