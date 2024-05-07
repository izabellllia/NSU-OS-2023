#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>
#include <poll.h>

#define BACKLOG 3

int fd = -1;
int accepted = -1;

void sigint_handler();
void finish();

int main() {
	atexit(finish);
	signal(SIGINT, sigint_handler);
	int len, status;
	struct sockaddr_un addr;

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("Socket error");
		exit(1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, "mysocket", sizeof(addr.sun_path)-1);

	if ((status = bind(fd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
		perror("Bind error");
		exit(1);
	}
	if (listen(fd, BACKLOG) == -1) {
		perror("Listen error");
		exit(1);
	}
    
    
    struct pollfd polls[BACKLOG];
    for (int i = 0; i < BACKLOG; ++i) {
        polls[i].fd = -1;
        polls[i].events = POLLIN;   
    }
    polls[0].fd = fd;

    int socketNumber = 0;


    while (1) {
        if (poll(polls, BACKLOG, -1) == -1) {
            perror("poll error ");
            exit(1);
        }


        for (int i = 0; i < BACKLOG; ++i) {
            if (polls[i].fd > 0) {
                if (polls[i].revents & POLLERR || polls[i].revents & POLLHUP || polls[i].revents & POLLNVAL) {
                    if (i == 0) {
                        perror("Server error");
                        exit(1);
                    } 

                    close(polls[i].fd);
                    polls[i].fd = -1;

                    --socketNumber;
                }                
            }
        }

        if (polls[0].revents & POLLIN && socketNumber < BACKLOG) {
            for (int i = 0; i < BACKLOG; i++) {
                if (polls[i].fd < 0) {
                    if ((polls[i].fd = accept(fd, NULL, NULL)) < 0) {
                        perror("accept error");
                        break;
                    }
                    ++socketNumber;
                }
            }
        }

        for (int i = 1; i < BACKLOG; i++) {
            if (polls[i].fd < 0) continue;

            if (!(polls[i].revents & POLLIN)) continue;

            char buffer[BUFSIZ];
            if ((len = read(polls[i].fd, buffer, BUFSIZ)) >= 0) {
                for (int k = 0; k < len; k++) {
                    buffer[k] = toupper(buffer[k]);
                }
                write(1, buffer, len);
            } else {
                perror("Read error");
                exit(1);
            }
            
        }    
    }
    
	exit(0);
}

void sigint_handler() {
    exit(0);
}

void finish() {
    if (fd != -1)
        close(fd);
    
    if (accepted != -1)
        close(accepted);
    
    unlink("mysocket");
}