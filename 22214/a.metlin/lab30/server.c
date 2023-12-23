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

int server_fd;

void sig_catch() {
	close(server_fd);

	if (unlink("socket_task30") == -1) {
		write(STDERR_FILENO, "Unlink error", 12); 
		exit(1);
	}
	exit(0);
}

int main() {
	sigset(SIGINT, sig_catch);
	sigset(SIGQUIT, sig_catch);
	struct sockaddr_un serv_addr;

	if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
			perror("Can't create socket");
			exit(1);
	}


	memset(&serv_addr, 0, sizeof(struct sockaddr_un));
	serv_addr.sun_family = AF_UNIX;
	strcpy(serv_addr.sun_path, "socket_task30");

	if (bind(server_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
			close(server_fd);
			perror("Can't bind socket to file");
			exit(3);
	}

	if (listen(server_fd, 1) == -1) {
			close(server_fd);
			unlink("socket_task30");
			perror("Can't listen socket");
			exit(4);
	}

	int client_socket = accept(server_fd, NULL, NULL);
	if (client_socket == -1) {
			close(server_fd);
			unlink("socket_task30");
			perror("Can't accept socket");
			exit(5);
	}


	char buf[BUFSIZ];
	int cnt;
	while ((cnt = read(client_socket, &buf, BUFSIZ)) > 0) {
			for(int i = 0; i < cnt; i++) {
					buf[i] = toupper(buf[i]);
			}
			write(STDOUT_FILENO, &buf, cnt);
	}

	if (cnt == -1) {
			close(client_socket);
			close(server_fd);
			unlink("socket_task30");
			perror("Can't read from socket");
			exit(6);
	}

	write(STDOUT_FILENO, "\n", 1);

	if (close(client_socket) == -1) {
			close(server_fd);
			unlink("socket_task30");
			perror("Can't close client_socket");
			exit(7);
	}
	if (close(server_fd) == -1) {
			unlink("socket_task30");
			perror("Can't close server_fd");
			exit(8);
	}
	if (unlink("socket_task30") == -1) {
			perror("Can't unlink socket filename");
			exit(9);
	}

	return 0;
}
