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

#define SOCKET_PATH "unix_socket30"

int server_socket;

int main() {
	
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
	
	if (listen(server_socket, 1) == -1) {
		perror("Can't listen socket");
		exit(4);
	}

	int client_socket = accept(server_socket, NULL, NULL);	
	if (client_socket == -1) {
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
		perror("Can't read from socket");
		exit(6);
	}

	write(STDOUT_FILENO, "\n", 1);
	
	if (close(client_socket) == -1) {
		perror("Can't close client_socket");
		exit(7);
	}
	if (close(server_socket) == -1) {
		perror("Can't close server_socket");
		exit(8);
	}
	if (unlink(SOCKET_PATH) == -1) {
		perror("Can't unlink socket filename");
		exit(9);
	}

	return 0;
}

void sig_catch() {
	close(server_socket);

	if (unlink(SOCKET_PATH) == -1) {
		write(STDERR_FILENO, "Unlink error", 12); 
		_exit(1);
	}
	_exit(0);
}

