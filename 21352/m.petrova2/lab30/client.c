#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>

char *socket_path = "./socket";

int main(int argc, char *argv[]) {
	struct sockaddr_un addr;
	char buf[BUFSIZ];
	int fileDesk;
	ssize_t rc;

	fileDesk = socket(AF_UNIX, SOCK_STREAM, 0);

	if (fileDesk == -1) {
		perror("client socket error");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

	if (connect(fileDesk, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("client connect error");
		return -1;
	}
	while((rc = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
		if (write(fileDesk, buf, rc) != rc) {
			if (rc > 0){
				fprintf(stderr,"partial write");
			}
			else {
				perror("write error");
				return -1;
			}
		}
	}

	return 0;
}
