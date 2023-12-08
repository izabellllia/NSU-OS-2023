#include<unistd.h>
#include<errno.h>
#include<stdio.h>
#include<sys/wait.h>
#include<stdlib.h>

int main(int argc, char** args) {
	
	if (argc < 2) {
		printf("Error: No arguments provided\n");
		exit(1);
	}	

	pid_t child_id = fork();
	
	if (child_id == -1) {
		perror("Can't create subprocess");
		exit(1);
	} else 
	if (child_id == 0) {
		 execvp(args[1], args + 1);
		 perror("Error while exec");	
		 exit(2); 
	} else 
	if (child_id > 0) {
		int status;
		int ret = wait(&status);
		if (ret == -1) {
			perror("Error while waiting subprocess");
			exit(3);
		}

		if (WIFEXITED(status)) {	
			printf("%d\n", WEXITSTATUS(status));
		} else {
			perror("Can't get exit status");
		}
	}

	return 0;
}
