#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

char *socket_path = "./socket";
void sigCatch(int sign) {
	unlink(socket_path);
	exit(1);
}

int main(int argc, char *argv[]) {
	struct sockaddr_un addr;
	char buf[BUFSIZ];
	int servDesk, clientDesk;
	ssize_t rc;

	servDesk = socket(AF_UNIX, SOCK_STREAM, 0);

	if (servDesk == -1) {
		perror("socket error");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);

	if (bind(servDesk, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		perror("bind error");
		return -1;
	}
	signal(SIGINT,sigCatch);

	if (listen(servDesk, 1) == -1) {
		unlink(socket_path);
		perror("listen error");
		return -1;
	}

	clientDesk = accept(servDesk, NULL, NULL);
	if (clientDesk  == -1) {
		unlink(socket_path);
		perror("accept error");
	}
	unlink(socket_path);

	while ((rc = read(clientDesk, buf, BUFSIZ))> 0) {
		for (int i = 0; i < rc; i++) {
			putchar(toupper(buf[i]));
		}
	}
	if (rc == -1) {
		perror("read");
		return -1;
	}
	else if (rc == 0) {
		close(clientDesk);
		return 0;
	}

	return 0;
}
