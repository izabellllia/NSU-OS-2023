#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "shell.h"

pid_t forkWrapper() {
  pid_t child = fork();
  if (child == -1) {
    perror("Failed to create child");
    exit(-1);
  }
  return child;
}

/*
argv - array of char* args (including command name) terminated by NULL
*/
void execWrapper(char** argv) {
	if (execvp(argv[0], argv) == -1) {
		perror("");
		exit(-1);
	}
}

void waitChild(pid_t childIndex) {
	int status;
	if (waitpid(childIndex, &status, 0) == -1) {
		perror("Error got while waiting for the child");
		exit(-1);
	}
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0 && errno != 0) {
		perror("");
	}
}

char *infile, *outfile, *appfile;
struct command cmds[MAXCMDS];
char bkgrnd;

int main(int argc, char *argv[])
{
	register int i;
	char line[1024];      /*  allow large command lines  */
	int ncmds;
	char prompt[50];      /* shell prompt */
	char buf[1024];

	/* PLACE SIGNAL CODE HERE */

	sprintf(prompt,"[%s] ", getenv("PWD"));

	while (promptline(prompt, line, sizeof(line)) > 0) {    /*until eof  */
		if ((ncmds = parseline(line)) <= 0)
			continue;   /* read next line */
#ifdef DEBUG
		{
			int i, j;
				for (i = 0; i < ncmds; i++) {
				for (j = 0; cmds[i].cmdargs[j] != (char *) NULL; j++)
					fprintf(stderr, "cmd[%d].cmdargs[%d] = %s\n",
					i, j, cmds[i].cmdargs[j]);
				fprintf(stderr, "cmds[%d].cmdflag = %o\n", i, cmds[i].cmdflag);
			}
		}
#endif

		for (i = 0; i < ncmds; i++) {
			pid_t childIndex = forkWrapper();
			if (childIndex == 0) {
				if (i == 0 && infile) {
					close(0);
					int inFd = open(infile, O_RDONLY);
					dup2(inFd, 0);
				}
				if (i == ncmds - 1 && (outfile || appfile)) {
					close(1);
					int flags = O_WRONLY | O_CREAT;
					char* path = outfile;
					if (appfile) {
						flags |= O_APPEND;
						path = appfile;
					}
					int outFd = open(path, flags, 0666);
					dup2(outFd, 1);
				}
				execWrapper(cmds[i].cmdargs);
			}
			else {
				if (!bkgrnd) {
					waitChild(childIndex);
					continue;
				}
				sprintf(buf, "Background process: %d %s\n", childIndex, cmds[i].cmdargs[0]);
				write(1, buf, strlen(buf));
			}
		}

	}  /* close while */
	return 0;
}

/* PLACE SIGNAL CODE HERE */