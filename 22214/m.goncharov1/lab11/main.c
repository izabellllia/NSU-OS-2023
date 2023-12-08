#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>

extern char** environ;

int execvpe(const char *file, char *argv[], char *envp[]) {
	char** mem = environ;
	environ = envp;
	execvp(file, argv);
	environ = mem;
	return -1;
}

int main() {
	
	char* args[] = {"date", NULL};
	char* nwEnvp[] = {"TZ=America/Los_Angeles", NULL};
	
	execvpe(args[0], args,  nwEnvp);
	
	perror("Error while exec");
	exit(1);
}
