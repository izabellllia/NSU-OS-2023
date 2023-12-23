#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<signal.h>

#define SOCKET_PATH "server31"

int server_socket;

int getLen(char* string) {
	int len = 0;
	while(string[len] != 0) len++;
	return len;
}

int main(int argc, char** args) {
	
	if (argc < 3) {
		printf("Enter args pls: 1 arg - message, 2 arg - interval in sec\n");
		exit(1);
	}
	
	int sleep_time;
	if (getLen(args[2]) == 1 && args[2][0] == '0') {
		sleep_time = 0;
	} else {
		sleep_time = atoi(args[2]);
		if (sleep_time == 0 || sleep_time < 0) {
			printf("Illegal interval\n");
			exit(1);
		}
	}
	

	void sig_catch();
	signal(SIGPIPE, sig_catch);

	server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_socket == -1) {
		perror("Can't create socket");
		exit(1);
	}
	
	struct sockaddr_un server_addr;
	server_addr.sun_family = AF_UNIX;
	strcpy(server_addr.sun_path, SOCKET_PATH);
	
	if (connect(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
		perror("Can't connect to server");
		exit(2);
	}
	
	for(int i = 0; i < 10; i++) {
		if (write(server_socket, args[1], strlen(args[1])) == -1) {
			perror("Can't write to server");
			exit(3);
		}
		sleep(sleep_time);
	}

	if (close(server_socket) == -1) {
		perror("Can't close connection");
		exit(4);
	}
	
	return 0;
}

void sig_catch() {
	write(STDOUT_FILENO, "Client sigpipe\n", 15);
	close(server_socket);
	_exit(1);
}
