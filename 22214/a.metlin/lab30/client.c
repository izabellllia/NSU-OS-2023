#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>

int main() {
    int client_fd;
    struct sockaddr_un serv_addr;
    char* msg = "Hello from client";
    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        printf("\n Socket creation error \n");
        exit(1);
    }

	memset(&serv_addr, 0, sizeof(struct sockaddr_un));
	serv_addr.sin_family = AF_UNIX;
	strcpy(serv_addr.sin_path, "socket_task30");

	if (connect(client_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
		perror("Can't connect to server");
		exit(2);
	}
	if (write(client_fd, msg, strlen(msg)) == -1) {
		perror("Can't write to server");
		exit(3);
	}
	if (close(client_fd) == -1) {
		perror("Can't close connection");
		exit(4);
	}
	return 0;
}