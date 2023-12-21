#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>

#define SOCKET_PATH "unix_socket30"

int main() {
	
	int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);
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
	
	char *msg = "Hello, world!";
	
	if (write(server_socket, msg, strlen(msg)) == -1) {
		perror("Can't write to server");
		exit(3);
	}
	
	if (close(server_socket) == -1) {
		perror("Can't close connection");
		exit(4);
	}
	
	return 0;
}
