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

int terminalDescriptor;
pid_t shellPgid;
struct termios defaultTerminalSettings;
static void initShell();

static int processJob(Job* job);

static int processShellSpecificCommand(Job* job);

// Naive places descriptor sourceFd on number from targetFd
static int substituteDescriptor(int sourceFd, int targetFd);

static void setInputOutputRedirection(Job* job, Process* process, int* prevPipes, int* newPipes);

static void waitChild(pid_t childID);

Job* fgJob;
char readInterruptionFlag = 0;
static void handleSigInt();

static void setSigIntHandler();

int main(int argc, char *argv[])
{
	Job* headJob = NULL;
	Job* bgJobHead = NULL;
	int i;
	char line[1024];      /*  allow large command lines  */
	int ncmds;
	char prompt[50];      /* shell prompt */
	Job* newJobsHead;
	Job* currentJob;
	Process* currentProcess;

	initShell();
	setSigIntHandler();

	sprintf(prompt, "[%s] ", getenv("PWD"));

	while (promptline(prompt, line, sizeof(line)) > 0) {
		if (readInterruptionFlag) {
			fprintf(stderr, "\n");
			readInterruptionFlag = 0;
			continue;
		}
		if ((newJobsHead = parseline(line)) == NULL)
			continue;
		writeJobs(newJobsHead);
		currentJob = newJobsHead;
		while (currentJob != NULL) {
			processJob(currentJob);
			currentJob = currentJob->next;
		}
		writeJobs(newJobsHead);
	}
	fprintf(stderr, "Bye!\n");
	// Зарегистрировать функцию для очистки памяти
	return 0;
}

void initShell() {
	// Initializing shell in separated group and set it as a foreground group for terminal
	terminalDescriptor = STDIN_FILENO;
	if (!isatty(terminalDescriptor)) {
		perror("Cannot get terminal");
		exit(-1);
	}
	// TODO: добавить проверку на то, что шэлл сам не был запущен в фоне
	shellPgid = getpid();
	if (setpgid(shellPgid, shellPgid) < 0) {
		perror("Cannot create shell's group");
		exit(-1);
	}
	tcsetpgrp(terminalDescriptor, shellPgid);
	// signal(SIGINT, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGCONT, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);	
	tcgetattr(terminalDescriptor, &defaultTerminalSettings);
}

int processJob(Job* job) {
	if (!processShellSpecificCommand(job))
		return 0;
	Process* currentProcess = job->headProcess;
	int prevPipes[2] = {-1, -1}, newPipes[2] = {-1, -1};
	while (currentProcess != NULL) {
		if (currentProcess->cmd.cmdflag)
			pipe(newPipes);
		pid_t childId = fork();
		if (childId < 0) {
			perror("Failed to launch process");
			return -1;
		}
		if (childId == 0) {
			childId = getpid();
			if (job->pgid == 0)
				job->pgid = childId;
			setpgid(childId, job->pgid);
			if (job->fg)
				tcsetpgrp(terminalDescriptor, job->pgid); // Если насчёт двойного setpgid были пояснения, то смысл двойного переназначения группы терминала остаётся загадочным

			pid_t childPgid = getpgid(0);

			// signal(SIGINT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);
			signal(SIGCONT, SIG_DFL);
			signal(SIGTTIN, SIG_DFL);
			signal(SIGTTOU, SIG_DFL);
			fprintf(stderr, "Child proccess %d in group %d\n", childId, childPgid);

			setInputOutputRedirection(job, currentProcess, prevPipes, newPipes);

			execvp(currentProcess->cmd.cmdargs[0], currentProcess->cmd.cmdargs);
			perror("Failed to run command");
			exit(-1);
		}
		else {
			currentProcess->pid = childId;
			if (job->pgid == 0)
				job->pgid = childId;
			setpgid(childId, job->pgid);

			close(prevPipes[0]);
			close(prevPipes[1]);
			prevPipes[0] = newPipes[0];
			prevPipes[1] = newPipes[1];

			if (job->fg) {
				fgJob = job;
				tcsetpgrp(terminalDescriptor, job->pgid);
				// To be continued...
			}
			else {
				fprintf(stderr, "Background process: %d %s\n", childId, currentProcess->cmd.cmdargs[0]);
				// To be continued...
			}
		}
		currentProcess = currentProcess->next;
	}
	if (job->fg) {
		waitChild(job->pgid);
		tcsetpgrp(terminalDescriptor, shellPgid);
		// To be continued...
	}
	else {
		// To be continued...
	}
	return 0;
}

int processShellSpecificCommand(Job* job) {
	return -1;
}

int substituteDescriptor(int sourceFd, int targetFd) {
	close(targetFd);
	return dup2(sourceFd, targetFd);
}

void setInputOutputRedirection(Job* job, Process* process, int* prevPipes, int* newPipes) {
	if (process == job->headProcess && job->inPath) {
		job->inFd = open(job->inPath, O_RDONLY);
		if (job->inFd < 0) {
			perror("Failed to redirect input");
			exit(-1);
		}
	}
	if (process->next == NULL && job->outPath) {
		int flags = O_WRONLY | O_CREAT;
		if (job->appendFlag)
			flags |= O_APPEND;
		job->outFd = open(job->outPath, flags, 0666);
		if (job->outFd < 0) {
			perror("Failed to redirect output");
			exit(-1);
		}
	}
	if (process->cmd.cmdflag & INPIP) {
		// fprintf(stderr, "Get input pipe from %d\n", prevPipes[0]);
		job->inFd = prevPipes[0];
	}
	if (process->cmd.cmdflag & OUTPIP) {
		// fprintf(stderr, "Get output pipe from %d\n", newPipes[1]);
		job->outFd = newPipes[1];
	}
	if (job->inFd != STDIN_FILENO)
		substituteDescriptor(job->inFd, STDIN_FILENO);
	if (job->outFd != STDOUT_FILENO)
		substituteDescriptor(job->outFd, STDOUT_FILENO);
	close(newPipes[0]);
	close(newPipes[1]);
	close(prevPipes[0]);
	close(prevPipes[1]);
}

void waitChild(pid_t childID) {
	siginfo_t statusInfo;
	// int options = WEXITED | WSTOPPED | (bkgrnd ? WNOHANG : 0);
	int options = WEXITED | WSTOPPED;
	if (waitid(P_PGID, childID, &statusInfo, options) == -1) {
		if (errno == EINTR) {
			fprintf(stderr, "Child process %d was interrupted by signal\n", childID);
		}
		else {
			perror("Error got while waiting for the child");
			exit(-1);
		}
	}
		// В дальнейшем тут может потребоваться дополнительная логика для прочих изменений состояния потомков
	if (statusInfo.si_code == CLD_EXITED) {
		if (statusInfo.si_status != 0) {
			fprintf(stderr, "Proccess %d exited with code %d\n", childID, statusInfo.si_status);
		}
	}
}

void handleSigInt() {
	if (fgJob) {
		sigsend(P_PGID, fgJob->pgid, SIGINT);
	}
}

void setSigIntHandler() {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	struct sigaction sigIntAction;
	sigIntAction.sa_handler = handleSigInt;
	sigIntAction.sa_mask = set;
	// sigIntAction.sa_flags = SA_RESTART; 
	// Сначала думал, что это хорошая идея - продолжать исполнение read и waitid, но в read это было бы весьма неудобно да и в waitid пока в этом нет нужды
	sigaction(SIGINT, &sigIntAction, NULL);
}