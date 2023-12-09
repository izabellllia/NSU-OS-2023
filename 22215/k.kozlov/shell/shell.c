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
#include <signal.h>
#include <setjmp.h>
#include <termios.h>

#include "shell.h"

/*
	Reference - https://www.gnu.org/software/libc/manual/html_node/Stopped-and-Terminated-Jobs.html
*/

static pid_t forkWrapper();

/*
argv - array of char* args (including command name) terminated by NULL
*/
static void execWrapper(char** argv);

// Naive places descriptor sourceFd on number from targetFd
static int substituteDescriptor(int sourceFd, int targetFd);

// Checks redirection options and substitutes corresponding std's descriptors
static void setInputOutputRedirection(int currentCmdIndex, int cmdsCount);

int pipesFds[MAXCMDS-1][2];

// Checks pipe's options and substitutes std's descriptors with pipe's in/out
static void setPipes(int cmdIndex, int ncmds);

// Wait child without bkgrnd option
static void waitChild(pid_t childID);

pid_t fgGroup = -1;
static void handleSigInt();

static void setSigIntHandler();

static int execShellSpecificCommand(char* line);

char *infile, *outfile, *appfile;
struct command cmds[MAXCMDS];
char bkgrnd;
char buf[1024];

int main(int argc, char *argv[])
{
	// Initializing shell in separated group and set it as a foreground group for terminal
	int terminalDescriptor = STDIN_FILENO;
	if (!isatty(terminalDescriptor))
		return 0;
	// TODO: добавить проверку на то, что шэлл сам не был запущен в фоне
	pid_t shellPgid = getpid();
	if (setpgid(shellPgid, shellPgid) < 0) {
		perror("Cannot create shell's group");
		return -1;
	}
	tcsetpgrp(terminalDescriptor, shellPgid);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGCONT, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	struct termios defaultTerminalSettings;
	tcgetattr(terminalDescriptor, &defaultTerminalSettings);

	int i;
	char prevLine[1024];
	char line[1024];      /*  allow large command lines  */
	int ncmds;
	char prompt[50];      /* shell prompt */

	setSigIntHandler();

	sprintf(prompt, "[%s] ", getenv("PWD"));

	while (promptline(prompt, line, sizeof(line)) > 0) {    /* until eof */
		if ((ncmds = parseline(line)) <= 0)
			continue;   /* read next line */
		for (i = 0; i < ncmds; i++) {
			// Opens pipe for i and i+1 commands
			if (cmds[i].cmdflag & OUTPIP) {
				pipe(pipesFds[i]);
			}
			
			pid_t childID = forkWrapper();
			if (childID == 0) {
				pid_t pid = getpid();
				setpgid(pid, pid);
				signal(SIGTSTP, SIG_DFL);
				signal(SIGCONT, SIG_DFL);
				signal(SIGTTIN, SIG_DFL);
				signal(SIGTTOU, SIG_DFL);
				sprintf(buf, "Child proccess %d\n", pid);
				write(2, buf, strlen(buf));
				setInputOutputRedirection(i, ncmds);
				setPipes(i, ncmds);
				execWrapper(cmds[i].cmdargs);
			}
			else {
				setpgid(childID, childID); // Необходимо для корректного перехода в новую группу процесса-потомка
				if (!bkgrnd) {
					fgGroup = childID;
					tcsetpgrp(terminalDescriptor, childID);
				}
				// Closes pipe for i-1 and i commands
				if (i > 0 && cmds[i].cmdflag & INPIP) {
					close(pipesFds[i-1][0]);
					close(pipesFds[i-1][1]);
				}

				waitChild(childID);
				if (!bkgrnd)
					tcsetpgrp(terminalDescriptor, shellPgid);
				sprintf(buf, "Waited %d %s\n", childID, cmds[i].cmdargs[0]);
				write(2, buf, strlen(buf));
				// Print message for command ran with &
				if (bkgrnd) {
					sprintf(buf, "Background process: %d %s\n", childID, cmds[i].cmdargs[0]);
					write(2, buf, strlen(buf));
				}
			}
		}
		// memcpy(prevLine, line, strlen(line)); // TODO: Заготовка для сохранения прошлых команд
	}  /* close while */
	return 0;
}

pid_t forkWrapper() {
  pid_t child = fork();
  if (child == -1) {
    perror("Failed to create child");
    exit(-1);
  }
  return child;
}

void execWrapper(char** argv) {
	if (execvp(argv[0], argv) == -1) {
		perror("");
		exit(-1);
	}
}

int substituteDescriptor(int sourceFd, int targetFd) {
	close(targetFd);
	return dup2(sourceFd, targetFd);
}

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

void setPipes(int cmdIndex, int ncmds) {
	if (cmdIndex < ncmds - 1 && (cmds[cmdIndex].cmdflag & OUTPIP)) {
		substituteDescriptor(pipesFds[cmdIndex][1], 1);
		close(pipesFds[cmdIndex][1]);
		close(pipesFds[cmdIndex][0]);
		sprintf(buf, "Proccess %d got pipe %d on stdout\n", cmdIndex, pipesFds[cmdIndex][1]);
		write(2, buf, strlen(buf));
	}
	if (cmdIndex > 0 && (cmds[cmdIndex].cmdflag & INPIP)) {
		substituteDescriptor(pipesFds[cmdIndex-1][0], 0);
		close(pipesFds[cmdIndex-1][0]);
		close(pipesFds[cmdIndex-1][1]);
		sprintf(buf, "Proccess %d got pipe %d on stdin\n", cmdIndex, pipesFds[cmdIndex-1][0]);
		write(2, buf, strlen(buf));
	}
}

void waitChild(pid_t childID) {
	siginfo_t statusInfo;
	int options = WEXITED | (bkgrnd ? WNOHANG : 0);
	if (waitid(P_PGID, childID, &statusInfo, options) == -1) {
		if (errno == EINTR) {
			sprintf(buf, "Child process %d was interrupted by signal\n", childID);
			write(2, buf, strlen(buf));
		}
		else {
			perror("Error got while waiting for the child");
			exit(-1);
		}
	}
		// В дальнейшем тут может потребоваться дополнительная логика для прочих изменений состояния потомков
	if (statusInfo.si_code == CLD_EXITED) {
		if (statusInfo.si_status != 0) {
			sprintf(buf, "Proccess %d exited with code %d\n", childID, statusInfo.si_status);
			write(2, buf, strlen(buf));
		}
	}
}

void handleSigInt() {
	if (fgGroup > 0) {
		sigsend(P_PGID, fgGroup, SIGINT);
	}
}

void setSigIntHandler() {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	sigaddset(&set, SIGTSTP);
	sigaddset(&set, SIGCONT);
	sigaddset(&set, SIGTTIN);
	sigaddset(&set, SIGTTOU);
	struct sigaction sigIntAction;
	sigIntAction.sa_handler = handleSigInt;
	sigIntAction.sa_mask = set;
	// sigIntAction.sa_flags = SA_RESTART; 
	// Сначала думал, что это хорошая идея - продолжать исполнение read и waitid, но в read это было бы весьма неудобно да и в waitid пока в этом нет нужды
	sigaction(SIGINT, &sigIntAction, NULL);
}