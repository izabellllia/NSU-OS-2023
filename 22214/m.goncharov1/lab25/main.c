#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/types.h>
#include<errno.h>
#include<stdlib.h>
#include<ctype.h>

int getLen(char* text) {
	int cnt = 0;
	while(text[cnt] != 0) {
		cnt++;
	}
	return cnt + 1;
}

int main() {
	
	int fds[2];
	if (pipe(fds) == -1) {
		perror("Error while creating pipe");
		exit(1);
	}

	pid_t pid = fork();
	if (pid == -1) {
		perror("Error while forking");
		exit(1);
	} else 
	if (pid != 0) {
		// parent
		close(fds[0]);
		char* text = "heLLo, WoRlD!";
		if (write(fds[1], text, getLen(text)) == -1) {
			perror("Error while writing to pipe");
			exit(1);
		}
		close(fds[1]);
	} else {
		// child
		close(fds[1]);
		char c;
		ssize_t status;
		while ((status = read(fds[0], &c, 1)) != 0) {
			if (status == -1) {
				perror("Error while reading from pipe");
				exit(1);
			} else {
				printf("%c", toupper(c));
			}
		}
		printf("\n");
		close(fds[0]);
	}
	
	return 0;
}
