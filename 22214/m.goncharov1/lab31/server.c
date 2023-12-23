#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<ctype.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<signal.h>
#include<poll.h>

#define SOCKET_PATH "server31"
#define MAX_CONNECTIONS 2

int server_socket;

int main() {
	
	char buf[BUFSIZ];
	void sig_catch();
	sigset(SIGINT, sig_catch);
	sigset(SIGQUIT, sig_catch);

	server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_socket == -1) {
		perror("Can't create socket");
		exit(1);
	}

	struct sockaddr_un server_addr;
	memset(&server_addr, 0, sizeof(struct sockaddr_un));
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCKET_PATH);

	if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		perror("Can't bind socket to file");
		exit(3);
	}
	
	if (listen(server_socket, MAX_CONNECTIONS) == -1) {
		perror("Can't listen socket");
		exit(4);
	}

	struct pollfd fds[MAX_CONNECTIONS + 1]; // + 1 for accept
	fds[0].fd = server_socket;
	fds[0].events = POLLIN;
	for(int i = 1; i < MAX_CONNECTIONS + 1; i++) {
		fds[i].fd = -1;
		fds[i].events = POLLIN;
	}	
	
	printf("Server: Hi!\n");

	while(1) {
		if (poll(fds, MAX_CONNECTIONS + 1, -1) == -1) {
			perror("Error while poll");
			exit(1);
		}
		
		for(int i = 1; i < MAX_CONNECTIONS + 1; i++) {
			if (fds[i].revents == 0) continue;
			if (fds[i].revents != POLLIN) {
				close(fds[i].fd);
				fds[i].fd = -1;
			} else {
				int cnt;
				cnt = read(fds[i].fd, buf, BUFSIZ - 1);
				if (cnt > 0) {
					for(int i = 0; i < cnt; i++) {
						buf[i] = toupper(buf[i]);
					}
					buf[cnt] = 0;
					printf("%s\n", buf);
				} else {
					close(fds[i].fd);
					fds[i].fd = -1;
				}
			}	
		}
		
		int freefdIdx = -1;	
		for(int i = 1 ; i < MAX_CONNECTIONS + 1; i++) {
			if (fds[i].fd == -1) {
				freefdIdx = i;
				break;
			}
		}
		// Check for new connection
		if (freefdIdx == -1 || fds[0].revents == 0) continue;
		
		if (fds[0].revents != POLLIN) {
			close(server_socket);
			if (unlink(SOCKET_PATH) == -1) {
				write(STDERR_FILENO, "Unlink error", 12); 
			}
			exit(1);
		} else {
			int client_socket = accept(server_socket, NULL, NULL);	
			if (client_socket == -1) {
				perror("Can't accept socket");
				exit(1);
			}
			printf("New client socket: %d\n", client_socket);
			fds[freefdIdx].fd = client_socket;
		}
	}
}

void sig_catch() {
	close(server_socket);

	if (unlink(SOCKET_PATH) == -1) {
		write(STDERR_FILENO, "Unlink error", 12); 
		_exit(1);
	}
	_exit(0);
}

