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
#include <termios.h>

#include "shell.h"

/*
	Reference - https://www.gnu.org/software/libc/manual/html_node/Stopped-and-Terminated-Jobs.html
*/

// TODO: задокументировать всё

int terminalDescriptor;
pid_t shellPgid;
struct termios defaultTerminalSettings;
static void initShell();

static int processJob(Job* job);

// Only one process in job!
static int processShellSpecificMainCommand(Job* job);

static int processShellSpecificForkedCommand(Process* process);

static int substituteDescriptor(int sourceFd, int targetFd);

static void setInputOutputRedirection(Job* job, Process* process, int* prevPipes, int* newPipes);

static void waitFgJob(Job* job);

int bgFreeNumber = 1;
Job* headBgJob = NULL;
Job* lastBgJob;
static int addJobToBg(Job* job);

char readInterruptionFlag = 0;
static void handleSigInt();

static void setSigIntHandler();

static void jobs_cmd();

static int parseFromIntFromPercentArg(char* arg);

static void fg_cmd(char* argNum);

int main(int argc, char *argv[])
{
	char line[1024];      /*  allow large command lines  */
	char prompt[50];      /* shell prompt */
	Job* newJobsHead;
	Job* currentJob;
	Job* nextJob;

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
		nextJob = newJobsHead;
		while (nextJob != NULL) {
			currentJob = nextJob;
			nextJob = nextJob->next;
			extractJobFromList(currentJob);
			if (!processShellSpecificMainCommand(currentJob))
				continue;
			processJob(currentJob);
			// currentJob = currentJob->next;
			// TODO: Опрашивать фоновые jobs'ы и выводить их статусы (возможно только неизменные)
		}
	}
	fprintf(stderr, "Bye!\n");
	// TODO: Чистить память от jobs'ов
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

	headBgJob = createNewJob(headBgJob);
	lastBgJob = headBgJob;
}

int processJob(Job* job) {
	Process* currentProcess = job->headProcess;
	Process* nextProcess = currentProcess;
	int prevPipes[2] = {-1, -1}, newPipes[2] = {-1, -1};
	while (nextProcess != NULL) {
		currentProcess = nextProcess;
		nextProcess = nextProcess->next;
		// Здесь надо будет обрабатывать shell-специфичные команды, для которых background исполнение в потомке не предусмотрено
		
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
				tcsetpgrp(terminalDescriptor, job->pgid);

			pid_t childPgid = getpgid(0);

			// signal(SIGINT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);
			signal(SIGCONT, SIG_DFL);
			signal(SIGTTIN, SIG_DFL);
			signal(SIGTTOU, SIG_DFL);
			// fprintf(stderr, "Child proccess %d in group %d\n", childId, childPgid);

			setInputOutputRedirection(job, currentProcess, prevPipes, newPipes);

			if (!processShellSpecificForkedCommand(currentProcess))
				exit(0);

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
		}
	}
	if (job->fg) {
		tcsetpgrp(terminalDescriptor, job->pgid);
		waitFgJob(job);
		tcsetpgrp(terminalDescriptor, shellPgid);
	}
	else {
		int bgNumber = addJobToBg(job);
		fprintf(stderr, "Background job %d:\n", bgNumber);
		printJobs(headBgJob);
		// To be continued...
	}
	return 0;
}

int processShellSpecificMainCommand(Job* job) {
	Process* process = job->headProcess;
	if (process->next || !process->cmd.cmdargs[1] || process->cmd.cmdargs[1][0] != '%') {
		fprintf(stderr, "Invalid syntax for shell-specific command\n");
		fprintf(stderr, "fg, bg, kill must be one in group\n");
		// Здесь по-хорошему должно быть какое-то особое завершние, потому что при значении NULL мы продолжем пытаться исполнять job
		return -1;
	}
	if (strcmp(process->cmd.cmdargs[0], "fg") == 0) {
		fg_cmd(process->cmd.cmdargs[1]);
		return 0;
	}
	else
		return -1;
}

int processShellSpecificForkedCommand(Process* process) {
	if (strcmp(process->cmd.cmdargs[0], "jobs") == 0) {
		jobs_cmd();
		return 0;
	}
	else 
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

void waitFgJob(Job* job) {
	siginfo_t statusInfo;
	int options = WEXITED | WSTOPPED;
	while (!isAllProcessesTerminated(job)) {
		if (waitid(P_PGID, job->pgid, &statusInfo, options) == -1) {
			if (errno == EINTR) {
				fprintf(stderr, "Child process %d was interrupted by signal\n", job->pgid);
			}
			else {
				perror("Error got while waiting for the child");
				exit(-1);
			}
		}
		Process* p = getProcessByPid(job, statusInfo.si_pid);
		p->statusInfo = statusInfo;
		fprintf(stderr, "Waited process from job %d:\n", job->pgid);
		printProcess(p);
		// TODO: Реализовать обработку остановку потомка по SIGTSTP
	}
}

int addJobToBg(Job* job) {
	job->bgNumber = bgFreeNumber;
	if (!headBgJob) {
		headBgJob = job;
		lastBgJob = job;
	}
	else {
		lastBgJob->next = job;
		job->prev = lastBgJob;
		lastBgJob = job;
	}
	return bgFreeNumber++;
}

// TODO: Сделать wait-опросник для фоновых процессов

void handleSigInt() {}

void setSigIntHandler() {
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGQUIT);
	struct sigaction sigIntAction;
	sigIntAction.sa_handler = handleSigInt;
	sigIntAction.sa_mask = set;
	// sigIntAction.sa_flags = SA_RESTART;
	sigaction(SIGINT, &sigIntAction, NULL);
}

// TODO: Реализовать более человеческий метод вывода
void jobs_cmd() {
	// TODO: Вызывать опросник
	printJobs(headBgJob->next);
}

static int parseFromIntFromPercentArg(char* arg) {
	int num;
	sscanf(arg+1, "%d", &num);
	return num;
}

void fg_cmd(char* argNum) {
	int bgNumber = parseFromIntFromPercentArg(argNum);
	Job* bgJob = getJobByBgNumber(headBgJob, bgNumber);
	if (!bgJob)	{
		fprintf(stderr, "Background job %d wasn't found\n", bgNumber);
		return;
	}
	extractJobFromList(bgJob);
	tcsetpgrp(terminalDescriptor, bgJob->pgid);
	sigsend(P_PGID, bgJob->pgid, SIGCONT);
	waitFgJob(bgJob);
	tcsetpgrp(terminalDescriptor, shellPgid);
}

// TODO: bg

// TODO: kill