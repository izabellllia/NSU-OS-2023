#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
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

void setInputOutputRedirection(int currentCmdIndex, int cmdsCount) {
	if (currentCmdIndex == 0 && infile) {
		close(0);
		int inFd = open(infile, O_RDONLY);
		dup2(inFd, 0);
	}
	if (currentCmdIndex == cmdsCount - 1 && (outfile || appfile)) {
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
}

int pipesFds[MAXCMDS-1][2];
void setPipes(int cmdIndex, int ncmds) {
	char buf[1024];
	int reservedStdOut = dup(1);
	int tmp;
	if (cmdIndex < ncmds - 1 && (cmds[cmdIndex].cmdflag & OUTPIP)) {
		close(1);
		tmp = dup2(pipesFds[cmdIndex][1], 1);
		close(pipesFds[cmdIndex][0]);
		// sprintf(buf, "Set pipe's descriptor %d on stdout of process %d (it's now %d)\n", pipesFds[cmdIndex][1], cmdIndex, tmp);
		// write(reservedStdOut, buf, strlen(buf));
	}
	if (cmdIndex > 0 && (cmds[cmdIndex].cmdflag & INPIP)) {
		close(0);
		tmp = dup2(pipesFds[cmdIndex-1][0], 0);
		close(pipesFds[cmdIndex-1][1]);
		// sprintf(buf, "Set pipe's descriptor %d on stdin of process %d (it's now %d)\n", pipesFds[cmdIndex-1][0], cmdIndex, tmp);
		// write(reservedStdOut, buf, strlen(buf));
	}
}

void waitChild(pid_t childID) {
	siginfo_t statusInfo;
	int options = WEXITED | (bkgrnd ? WNOHANG : 0);
	char buf[1024] = "";
	if (waitid(P_PID, childID, &statusInfo, options) == -1) {
		perror("Error got while waiting for the child");
		exit(-1);
	}
	if (statusInfo.si_code == CLD_EXITED && statusInfo.si_status != 0) {
		sprintf(buf, "Proccess exited with code %d\n", statusInfo.si_status);
		write(1, buf, strlen(buf));
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
			if (cmds[i].cmdflag & OUTPIP) {
				pipe(pipesFds[i]);
				// sprintf(buf, "Create pipe: %d %d\n", pipesFds[i][0], pipesFds[i][1]);
				// write(1, buf, strlen(buf));
			}
			pid_t childID = forkWrapper();
			if (childID == 0) {
				// if (bkgrnd) setpgid(0, 0);
				setInputOutputRedirection(i, ncmds);
				setPipes(i, ncmds);
				execWrapper(cmds[i].cmdargs);
			}
			else {
				if (i > 0) {
					close(pipesFds[i-1][0]);
					close(pipesFds[i-1][1]);
				}
				waitChild(childID);
				if (bkgrnd) {
					sprintf(buf, "Background process: %d %s\n", childID, cmds[i].cmdargs[0]);
					write(1, buf, strlen(buf));
				}
			}
		}

	}  /* close while */
	return 0;
}

/* PLACE SIGNAL CODE HERE */