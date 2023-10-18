#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <errno.h>

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

main(int argc, char *argv[])
{
	register int i;
	char line[1024];      /*  allow large command lines  */
	int ncmds;
	char prompt[50];      /* shell prompt */

	/* PLACE SIGNAL CODE HERE */

	sprintf(prompt,"[%s] ", argv[0]);

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
			if (childIndex == 0)
				execWrapper(cmds[i].cmdargs);
			else
				waitChild(childIndex);	
		}

	}  /* close while */
}

/* PLACE SIGNAL CODE HERE */