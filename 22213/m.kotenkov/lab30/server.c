#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define SERVER_SOCK_FILE "server.sock"
#define MSGSIZE 20


int main() {
	int fd, len, status;
	struct sockaddr_un addr;
	char msgin[MSGSIZE];
	memset(msgin, '\0', MSGSIZE);

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("Socket creation failure");
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, SERVER_SOCK_FILE);
	
	unlink(SERVER_SOCK_FILE);
	if ((status = bind(fd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
		perror("Binding failure");
		exit(EXIT_FAILURE);
	}
	if (listen(fd, 1) == -1) {
		perror("Listening failure");
	}
	
	int accepted;
	if ((accepted = accept(fd, NULL, NULL)) == -1) {
		perror("Accept failed");
	}
		
	while ((len = read(accepted, msgin, MSGSIZE)) > 0) {
		for (int i = 0; i < len; i++) {
			msgin[i] = toupper(msgin[i]);
		}
		if ((write(1, msgin, len)) == -1) {
			perror("Write failure");
			exit(EXIT_FAILURE);
		};
	};

	if (len == -1) {
		perror("Read failure");
		exit(EXIT_FAILURE);
	}

	close(accepted);
	close(fd);
	unlink(SERVER_SOCK_FILE);
	exit(EXIT_SUCCESS);
}
