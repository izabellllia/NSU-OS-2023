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

// Naive places descriptor sourceFd on number from targetFd
int substituteDescriptor(int sourceFd, int targetFd) {
	close(targetFd);
	return dup2(sourceFd, targetFd);
}

// Checks redirection options and substitutes corresponding std's descriptors
void setInputOutputRedirection(int currentCmdIndex, int cmdsCount) {
	if (currentCmdIndex == 0 && infile) {
		int inFd = open(infile, O_RDONLY);
		substituteDescriptor(inFd, 0);
	}
	if (currentCmdIndex == cmdsCount - 1 && (outfile || appfile)) {
		int flags = O_WRONLY | O_CREAT;
		char* path = outfile;
		if (appfile) {
			flags |= O_APPEND;
			path = appfile;
		}
		int outFd = open(path, flags, 0666);
		substituteDescriptor(outFd, 1);
	}
}

int pipesFds[MAXCMDS-1][2];

// Checks pipe's options and substitutes std's descriptors with pipe's in/out
void setPipes(int cmdIndex, int ncmds) {
	if (cmdIndex < ncmds - 1 && (cmds[cmdIndex].cmdflag & OUTPIP)) {
		substituteDescriptor(pipesFds[cmdIndex][1], 1);
		close(pipesFds[cmdIndex][0]);
	}
	if (cmdIndex > 0 && (cmds[cmdIndex].cmdflag & INPIP)) {
		substituteDescriptor(pipesFds[cmdIndex-1][0], 0);
		close(pipesFds[cmdIndex-1][1]);
	}
}

// Wait child without bkgrnd option
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
	int i;
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
			// Opens pip for i and i+1 commands
			if (cmds[i].cmdflag & OUTPIP) {
				pipe(pipesFds[i]);
			}
			
			pid_t childID = forkWrapper();
			if (childID == 0) {
				// if (bkgrnd) setpgid(0, 0); // First step to jobs
				setInputOutputRedirection(i, ncmds);
				setPipes(i, ncmds);
				execWrapper(cmds[i].cmdargs);
			}
			else {
				// Closes pipe for i-1 and i commands
				if (cmds[i-1].cmdflag & INPIP) {
					close(pipesFds[i-1][0]);
					close(pipesFds[i-1][1]);
				}

				waitChild(childID);
				
				// Print message for command ran with &
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