#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <string.h>

int fd = -1;
int accepted = -1;

void sigint_handler();
void finish();

int main() {
	atexit(finish);
	signal(SIGINT, sigint_handler);
	int len, status;
	struct sockaddr_un addr;

	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		perror("Socket error");
		exit(1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, "mysocket", sizeof(addr.sun_path)-1);

	if ((status = bind(fd, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
		perror("Bind error");
		exit(1);
	}
	if (listen(fd, 1) == -1) {
		perror("Listen error");
		exit(1);
	}

	int accepted;
	if ((accepted = accept(fd, NULL, NULL)) == -1) {
		perror("Accept error");
		exit(1);
	}
		
	char symbol;
	char buffer[BUFSIZ];
	int capacity = BUFSIZ;
	while ((len = read(accepted, &symbol, 1)) > 0) {
		buffer[BUFSIZ - capacity] = toupper(symbol);
		--capacity;
		if (capacity == 0) {
			if ((write(1, buffer, BUFSIZ)) == -1) {
				perror("Write error");
				exit(1);
			};
			capacity = BUFSIZ;
		}
	};

	if (len == -1) {
		perror("Read error");
		exit(1);
	} else if (len == 0 && capacity < BUFSIZ)
		write(1, buffer, BUFSIZ - capacity);


	exit(0);
}

void sigint_handler() {
    exit(0);
}

void finish() {
    if (fd != -1)
        close(fd);
    
    if (accepted != -1)
        close(accepted);
    
    unlink("mysocket");
}