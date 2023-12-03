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

#include "shell.h"

char buf[1024];

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

char *infile, *outfile, *appfile;
struct command cmds[MAXCMDS];
char bkgrnd;

int main(int argc, char *argv[])
{
	pid_t sid = setsid();
	// int terminalDescriptor = open("/dev/tty", O_RDWR);
	int i;
	char prevLine[1024];
	char line[1024];      /*  allow large command lines  */
	int ncmds;
	char prompt[50];      /* shell prompt */

	setSigIntHandler();

	sprintf(prompt, "[%s] ", getenv("PWD"));

	while (promptline(prompt, line, sizeof(line)) > 0) {    /* until eof */
		// memcpy(prevLine, line, strlen(line)); // TODO: Заготовка для сохранения прошлых команд
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
				if (bkgrnd) setpgid(0, 0);
				pid_t pid = getpid();
				sprintf(buf, "Child proccess %d\n", pid);
				write(1, buf, strlen(buf));
				setInputOutputRedirection(i, ncmds);
				setPipes(i, ncmds);
				execWrapper(cmds[i].cmdargs);
			}
			else {
				if (!bkgrnd) {
					fgGroup = childID;
					// tcsetpgrp(0, fgGroup); // TODO: Теоретически, должно работать корректно, но надо искать больше инфы про процессы первого плана
				}
				// Closes pipe for i-1 and i commands
				if (i > 0 && cmds[i].cmdflag & INPIP) {
					close(pipesFds[i-1][0]);
					close(pipesFds[i-1][1]);
				}

				waitChild(childID);
				
				// Print message for command ran with &
				if (bkgrnd) {
					sprintf(buf, "Background process: %d %s\n", childID, cmds[i].cmdargs[0]);
					write(2, buf, strlen(buf));
				}
			}
		}

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
	if (waitid(P_PID, childID, &statusInfo, options) == -1) {
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

/*
Как убиваются зомби от фоновых процессов?
Через настройки jobs-ов
https://www.gnu.org/software/libc/manual/html_node/Stopped-and-Terminated-Jobs.html

Что такое сигнал?
Для чего нужны сигналы?
Что значит "обработать сигнал"?
Кто кому может посылать сигналы?
Как посылаются сигналы?
*/

void handleSigInt() {
	if (fgGroup > 0) {
		sigsend(P_PGID, fgGroup, SIGINT);
	}
	sprintf(buf, "\nSIGINT handled\n");
	write(2, buf, strlen(buf));
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
