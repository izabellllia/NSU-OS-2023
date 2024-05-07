#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv) {
	pid_t pid, ret;
	int status;

	if(argc < 2) {
		fprintf(stderr, "No text file\n");
		return 0;
	}
	pid = fork();
	if (pid == -1) {
		perror("Fork error");
		return 0;
	}
	if(pid == 0) {
		printf("child process starts\n");
		execlp("cat", "cat",argv[1], (char *) 0);
		perror("Cat failed");
		exit(1);
	}
	if(pid > 0) {
		ret = wait(&status);
		if(ret == -1){
			perror("Wait error");
			return 0;
		}
		printf("parent: return value=%ld\n", ret);
		if (WIFEXITED(status))
			printf("child's exit status is: %d\n",WEXITSTATUS(status));
		else
			if (WIFSIGNALED(status))
				printf("signal is: %d\n", WTERMSIG(status));
	}
	return 0;
}

