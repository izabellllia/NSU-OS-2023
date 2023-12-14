 #include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>

char *socket_path = "./socket";

void signal_handle(int signo) {
	unlink(socket_path);
	_Exit(0);
}

int main(int argc, char *argv[]) {
	struct sockaddr_un addr;
	int server_descriptor, client_descriptor;

	server_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);
	if (server_descriptor == -1) {
		perror("socket error");
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);


	if (bind(server_descriptor, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind error");
		exit(EXIT_FAILURE);
	}

	signal(SIGINT, signal_handle);

	if (listen(server_descriptor, 5) == -1) {
		unlink(socket_path);
		perror("listen error");
		exit(EXIT_FAILURE);
	}

	if ((client_descriptor = accept(server_descriptor, NULL, NULL)) == -1) {
		unlink(socket_path);
		perror("accept error");
		exit(EXIT_FAILURE);
	}

	unlink(socket_path);


	char buffer[BUFSIZ];
	ssize_t read_bytes;

	while( (read_bytes = read(client_descriptor, buffer, sizeof(buffer))) > 0) {
		for (int i = 0; i < read_bytes; i++) {
			putchar(toupper(buffer[i]));
		}
	}

	if (read_bytes == -1) {
		perror("read error");
		exit(EXIT_FAILURE);
	} else if (read_bytes == 0) {
        exit(EXIT_SUCCESS);
	}
}
