#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/poll.h>

#define MAX_CLIENTS 10
#define BUFSIZE 2047

char socket_path[] = "just_socket";

void handler(int sign) {
    write(STDIN_FILENO, "\n", 1);
    unlink(socket_path);
    exit(0);
}

int main() {
    sigset(SIGINT, handler); 

    char buffer[BUFSIZE];
    struct sockaddr_un addr;
    int server_descriptor;
    
    server_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_descriptor == -1) {
        perror("socket error");
        return - 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (bind(server_descriptor,(struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(server_descriptor);
        perror("bind error");
        return -1;
    }

    if (listen(server_descriptor, 10) == -1) {
        unlink(socket_path);
        close(server_descriptor);
        perror("listen error");
        return -1;
    }

    struct pollfd fds[MAX_CLIENTS + 1];  // +1 for the server socket
    memset(fds, 0, sizeof(fds));
    fds[0].fd = server_descriptor;
    fds[0].events = POLLIN;    

    int num_clients = 0;

    while (1) {
        if (poll(fds, num_clients + 1, -1) == -1) {
            unlink(socket_path);
            perror("Poll failed");
            exit(-1);
        }

        for (int i = 1; i <= num_clients; i++) {
            if (fds[i].revents & POLLIN) {
                size_t bytesRead = read(fds[i].fd, buffer, BUFSIZE);

                if (bytesRead <= 0) {
                    close(fds[i].fd);
                    fds[i] = fds[num_clients];
                    fds[num_clients].fd = 0;
                    fds[num_clients].events = 0;
                    num_clients--; //active
                    fds[0].events = POLLIN;

                } else {
                    for (size_t j = 0; j < bytesRead; j++) {
                        buffer[j] = toupper(buffer[j]);
                    }
                    write(STDIN_FILENO, buffer, bytesRead);
                }
            }
        }

        if (fds[0].revents & POLLIN) {
            int clientFd;
            if ((clientFd = accept(server_descriptor, NULL, NULL)) == -1) {
                perror("Accept failed");
                unlink(socket_path);
                exit(-1);
            }
            num_clients++;
            fds[num_clients].fd = clientFd;
            fds[num_clients].events = POLLIN;
            if (num_clients == MAX_CLIENTS) {
                fds[0].events = 0;
            }
        }

    }
    
    return 0;
}
