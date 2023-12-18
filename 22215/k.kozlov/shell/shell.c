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
#include "shell_structs.h"
#include "jobs.h"
#include "shell_cmds.h"

// TODO: задокументировать всё

static char* updatePrompt();

int terminalDescriptor;
pid_t shellPgid;
struct termios defaultTerminalSettings;
static void initShell();

static int processJob(Job* job);

static int substituteDescriptor(int sourceFd, int targetFd);

static void setInputOutputRedirection(Job* job, Process* process, int* prevPipes, int* newPipes);

int bgFreeNumber = 1;
Job* headBgJobFake = NULL;
static int addJobToBg(Job* job);

char readInterruptionFlag = 0;
static void handleSigInt();
static void setSigIntHandler();

char prompt[1024];

int main(int argc, char *argv[])
{
	char line[1024];      /*  allow large command lines  */
	Job* newJobsHead;
	Job* currentJob;
	Job* nextJob;

	initShell();

	while (promptline(updatePrompt(), line, sizeof(line)) > 0) {
		if (readInterruptionFlag) {
			fprintf(stderr, "\n");
			readInterruptionFlag = 0;
			continue;
		}
		if ((newJobsHead = parseline(line)) == NULL)
			continue;
		// printJobs(newJobsHead);
		nextJob = newJobsHead;
		while (nextJob != NULL) {
			updateJobsStatuses(headBgJobFake->next);

			currentJob = nextJob;
			nextJob = nextJob->next;
			extractJobFromList(currentJob);
			if (!processShellSpecificMainCommand(currentJob))
				processJob(currentJob);
			
			updateJobsStatuses(headBgJobFake->next);
			printJobsNotifications(headBgJobFake->next, 1);
			cleanJobs(headBgJobFake->next);
		}
	}
	if (headBgJobFake->next) {
		fprintf(stderr, "\nThese background jobs will get SIGHUP:\n");
		sendSigHups(headBgJobFake->next);
		updateJobsStatuses(headBgJobFake->next);
		printJobsNotifications(headBgJobFake->next, 0);
	}
	fprintf(stderr, "Bye!\n");
	freeJobs(headBgJobFake);
	return 0;
}

char* updatePrompt() {
	char buf[1017] = "";
	getcwd(buf, sizeof(buf));
	sprintf(prompt, "\n[%s]\n$ ", buf);
	return prompt;
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
	signal(SIGTSTP, SIG_IGN);
	signal(SIGCONT, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);	
	setSigIntHandler();
	tcgetattr(terminalDescriptor, &defaultTerminalSettings);
	headBgJobFake = createNewJob(headBgJobFake);
}

int processJob(Job* job) {
	Process* currentProcess = job->headProcess;
	Process* nextProcess = currentProcess;
	int prevPipes[2] = {-1, -1}, newPipes[2] = {-1, -1};
	while (nextProcess != NULL) {
		currentProcess = nextProcess;
		nextProcess = nextProcess->next;
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
			if (job->initialFg)
				tcsetpgrp(terminalDescriptor, job->pgid);

			signal(SIGINT, SIG_DFL);
			signal(SIGTSTP, SIG_DFL);
			signal(SIGCONT, SIG_DFL);
			signal(SIGTTIN, SIG_DFL);
			signal(SIGTTOU, SIG_DFL);

			setInputOutputRedirection(job, currentProcess, prevPipes, newPipes);

			if (processShellSpecificForkedCommand(currentProcess))
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
	if (job->initialFg) {
		tcsetpgrp(terminalDescriptor, job->pgid);
		waitFgJob(job);
		tcsetpgrp(terminalDescriptor, shellPgid);
		tcsetattr(terminalDescriptor, TCSAFLUSH, &defaultTerminalSettings);
	}
	else {
		addJobToBg(job);
	}
	return 0;
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
	while (!isAllProcessesEnded(job)) {
		if (waitid(P_PGID, job->pgid, &statusInfo, options) == -1) {
			if (errno == EINTR) {
				fprintf(stderr, "Wait was interrupted by signal\n");
			}
			else {
				perror("Error got while waiting for the child");
				exit(-1);
			}
		}
		Process* p = getProcessByPid(job, statusInfo.si_pid);
		p->statusInfo = statusInfo;
		if (p->statusInfo.si_code == CLD_EXITED && p->statusInfo.si_status != 0) {
			fprintf(stderr, "Process ended with exit code %d\n", p->statusInfo.si_status);
		}
		if (p->statusInfo.si_code == CLD_KILLED) {
			fprintf(stdout, "\n");
		}
		if (p->statusInfo.si_code == CLD_STOPPED) {
			extractJobFromList(job);
			addJobToBg(job);
			break;
		}
	}
	if (isAllProcessesEnded(job)) {
		extractJobFromList(job);
		freeJob(job);
	}
}

int addJobToBg(Job* job) {
	job->bgNumber = bgFreeNumber;
	Job* lastBgJob = headBgJobFake;
	while (lastBgJob->next != NULL) {
		lastBgJob = lastBgJob->next;
	}
	lastBgJob->next = job;
	job->prev = lastBgJob;
	return bgFreeNumber++;
}

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