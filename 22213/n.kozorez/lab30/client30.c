#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

char *socket_path = "./socket";

int main(int argc, char *argv[]) {

	struct sockaddr_un socket_address;

	int file_descriptor;

	file_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);
	if (file_descriptor == -1) {
		perror("client: failed to create socket");
		exit(EXIT_FAILURE);
	}

	memset(&socket_address, 0, sizeof(socket_address));
	socket_address.sun_family = AF_UNIX;
	strncpy(socket_address.sun_path, socket_path, sizeof(socket_address.sun_path) - 1);

	if (connect(file_descriptor, (struct sockaddr*)&socket_address, sizeof(socket_address)) == -1) {
		perror("connect error");
		exit(EXIT_FAILURE);
	}

	char buffer[BUFSIZ];
	ssize_t read_bytes;

	memset(buffer, 0, sizeof(buffer));
	while ((read_bytes = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
		if (write(file_descriptor, buffer, read_bytes) != read_bytes) {
			if (read_bytes > 0) {
				fprintf(stderr, "partial write");
			} else {
				perror("write error");
				exit(EXIT_FAILURE);
			}
		}
	}
	exit(EXIT_SUCCESS);
}
