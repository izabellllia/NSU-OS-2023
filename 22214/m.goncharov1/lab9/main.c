#include<stdio.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>

int main() {
	
	pid_t child_id = fork();

	switch (child_id) {
		case -1:
		{
			perror("Error while creating subprocess");
			exit(1);
		}
		case 0:
		{
			// this is subprocces
			int ret = execlp("cat", "cat", "text.txt", NULL);
			if (ret == -1) {
				perror("Error while calling cat");
				exit(2);
			}
		}
		default: {
			// this is parent
			wait(NULL);
			printf("\n\n===\nI'm parent\n===\n");
		}
	}

	exit(0);
}
