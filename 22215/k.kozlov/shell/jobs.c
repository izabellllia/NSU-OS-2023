#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <wordexp.h>

#include "shell_structs.h"
#include "jobs.h"

Job* createNewJob(Job* headJob) {
	Job* newJob = (Job*) malloc(sizeof(Job));
	newJob->headProcess = NULL;
	newJob->pgid = 0;
	newJob->initialFg = 1;
	newJob->bgNumber = 0;
	newJob->inFd = STDIN_FILENO;
	newJob->outFd = STDOUT_FILENO;
	newJob->inPath = NULL;
	newJob->outPath = NULL;
	newJob->appendFlag = 0;
	newJob->status = J_SETTING;
	newJob->notified = 0;
	newJob->prev = NULL;
	newJob->next = NULL;
	
	if (headJob == NULL)
		return newJob;
	Job* currentJob = headJob;
	while (currentJob->next != NULL) {
		currentJob = currentJob->next;
	}

	currentJob->next = newJob;
	newJob->prev = currentJob;
	return newJob;
}

int isAnySpace(char* str) {
	for (int i = 0; str[i]; ++i) {
		if (str[i] == ' ')
			return 1;
	}
	return 0;
}

void constructWordExp(char** args, wordexp_t *p) {
	char line[LINE_SZ], isSpace = 0;
	strcat(line, args[0]);
	for (int argIndex = 1; args[argIndex]; argIndex++) {
		isSpace = isAnySpace(args[argIndex]);
		strcat(line, " ");
		if (isSpace)
			strcat(line, "\"");
		strcat(line, args[argIndex]);
		if (isSpace)
			strcat(line, "\"");
	}
	wordexp(line, p, 0);
}

Process* createNewProcessInJob(Job* job, Command cmd) {
	Process* newProcess = (Process*) malloc(sizeof(Process));
	newProcess->cmd.cmdflag = cmd.cmdflag;
	newProcess->pipesFlags = cmd.cmdflag;

	int argLen = 0;
	for (int cmdArgIndex = 0; cmdArgIndex < MAXARGS; ++cmdArgIndex) {
		if (cmd.cmdargs[cmdArgIndex]) {
			argLen = strlen(cmd.cmdargs[cmdArgIndex]);
			newProcess->cmd.cmdargs[cmdArgIndex] = (char*) malloc(argLen + 1);
			strcpy(newProcess->cmd.cmdargs[cmdArgIndex], cmd.cmdargs[cmdArgIndex]);
			newProcess->cmd.cmdargs[cmdArgIndex][argLen] = '\0';
		}
		else {
			newProcess->cmd.cmdargs[cmdArgIndex] = (char*) NULL;
		}
	}
	strcat(newProcess->line, cmd.cmdargs[0]);
	for (int argIndex = 1; cmd.cmdargs[argIndex]; argIndex++) {
		strcat(newProcess->line, " ");
		strcat(newProcess->line, cmd.cmdargs[argIndex]);
	}
	wordexp(newProcess->line, &newProcess->args, 0);
	// constructWordExp(cmd.cmdargs, &newProcess->args);
	newProcess->pid = 0;
	newProcess->statusInfo.si_code = 0;
	newProcess->statusInfo.si_status = 0;
	newProcess->prev = NULL;	
	newProcess->next = NULL;	

	if (job->headProcess == NULL) {
		job->headProcess = newProcess;
		return newProcess;
	}
	Process* currentProcess = job->headProcess;
	while (currentProcess->next != NULL) {
		currentProcess = currentProcess->next;
	}
	currentProcess->next = newProcess;
	newProcess->prev = currentProcess;
	return newProcess;
}

void extractJobFromList(Job* job) {
	Job* prevJob = job->prev;
	Job* nextJob = job->next;
	job->prev = NULL;
	job->next = NULL;
	if (prevJob)
		prevJob->next = nextJob;
	if (nextJob)
		nextJob->prev = prevJob;
}

Job* getJobByBgNumber(Job* headJob, int bgNumber) {
	for (Job* job = headJob; job != NULL; job = job->next) {
		if (job->bgNumber == bgNumber)
			return job;
	}
	return NULL;
}

Process* getProcessByPid(Job* job, pid_t pid) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->pid == pid)
			return proc;
	}
	return NULL;
}

void updateJobStatus(Job* job) {
	char oldStatus = job->status;
	// if (isJobRunning(job))
	// 	job->status = J_RUNNING;
	if (isJobDone(job))
		job->status = J_DONE;
	else if (isJobExited(job))
		job->status = J_EXIT;
	else if (isJobTerminated(job))
		job->status = J_TERMINATED;
	else if (isJobStopped(job))
		job->status = J_STOPPED;
	else
		job->status = J_RUNNING;
	if (oldStatus != job->status)
		job->notified = 0;
}

void updateJobSiginfo(Job* job) {
	siginfo_t newStatusInfo;
	int options = WEXITED | WSTOPPED | WCONTINUED | WNOHANG;
	while (1) {
		if (waitid(P_PGID, job->pgid, &newStatusInfo, options) != 0) {
			if (errno != ECHILD)
				perror("Error in bg jobs refining");
			return;
		}
		if (newStatusInfo.si_pid == 0) {
			return;
		}
		Process* p = getProcessByPid(job, newStatusInfo.si_pid);
		p->statusInfo = newStatusInfo;
	}
}

void updateJobsStatuses(Job* headJob) {
	for (Job* job = headJob; job != NULL; job = job->next) {
		updateJobSiginfo(job);
		updateJobStatus(job);
	}
}

void cleanJobs(Job* headJob) {
	Job* job;
	Job* nextJob = headJob;
	while (nextJob != NULL) {
		job = nextJob;
		nextJob = job->next;
		if (isAllProcessesEnded(job) && job->notified) {
			extractJobFromList(job);
			freeJob(job);
		}
	}
}

int isAllProcessesEnded(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code != CLD_EXITED && proc->statusInfo.si_code != CLD_KILLED)
			return 0;
	}
	return 1;
}

int isJobRunning(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code == 0 || proc->statusInfo.si_code == CLD_CONTINUED)
			return 1;
	}
	return 0;
}

int isJobDone(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code != CLD_EXITED || proc->statusInfo.si_status != 0)
			return 0;
	}
	return 1;
}

int isJobExited(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code == CLD_EXITED && proc->statusInfo.si_status != 0)
			return 1;
	}
	return 0;
}

int isJobTerminated(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code == CLD_KILLED)
			return 1;
	}
	return 0;
}

int isJobStopped(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code == CLD_STOPPED)
			return 1;
	}
	return 0;
}

void printJobs(Job* headJob) {
	for (Job* job = headJob; job != NULL; job = job->next) {
		printJob(job);
		printProcesses(job);
	}
}

void printJob(Job* job) {
	if (job->bgNumber)
		fprintf(stderr, "BG(%d) ", job->bgNumber); 
	fprintf(stderr, "Job with group ID %d, IN = %s, OUT = %s%s:\n", 
		job->pgid, 
		job->inPath ? job->inPath : "STDIN", 
		job->appendFlag ? "+ " : "", 
		job->outPath ? job->outPath : "STDOUT"
	);
}

void printProcesses(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		printProcess(proc);
	}
}

void printProcess(Process* process) {
	fprintf(stderr, "\tProcess %d, PIPES %d\n", process->pid, process->cmd.cmdflag);
	fprintf(stderr, "\tCode %d\n\tStatus %d\n", 
		process->statusInfo.si_code, process->statusInfo.si_status);
	// for (int argIndex = 0; process->cmd.cmdargs[argIndex]; argIndex++) {
	// 	fprintf(stderr, "\t\tArg %d: %s\n", argIndex, process->cmd.cmdargs[argIndex]);
	// }
	for (int argIndex = 0; argIndex < process->args.we_wordc; argIndex++) {
		fprintf(stderr, "\t\tArg %d: %s\n", argIndex, process->args.we_wordv[argIndex]);
	}
}

void printJobsNotifications(Job* headJob, int onlyNotNotified) {
	for (Job* job = headJob; job != NULL; job = job->next) {
		if (onlyNotNotified && job->notified)
			continue;
		printJobNotification(job);
		printProcessesNotification(job);
	}
}

static const char* STATUS_MESSAGES[100] = {
	"Not launched", 
	"Running",
	"Done",
	"Exit",
	"Terminated",
	"Stopped"
};
void printJobNotification(Job* job) {
	fprintf(stdout, "[%d]   %d   (%s%s)\n", 
		job->bgNumber, job->pgid,
		job->notified ? "" : "+ ",
		STATUS_MESSAGES[(int)job->status]
	);
	job->notified = 1;
}

void printProcessesNotification(Job* job) {
	for (Process* proc = job->headProcess; proc; proc = proc->next) {
		// printProcessNotification(proc);
		fprintf(stdout, "\t%s\n", proc->line);
	}
}

void printProcessNotification(Process* process) {
	fprintf(stdout, "\t");
	for (int argIndex = 0; process->cmd.cmdargs[argIndex]; argIndex++) {
		fprintf(stdout, "%s ", process->cmd.cmdargs[argIndex]);
	}
	fprintf(stdout, "\n");
}

void freeJobs(Job* headJob) {
	Job* currJob = headJob;
	Job* jobForDeletion;
	while (currJob)
	{
		jobForDeletion = currJob;
		currJob = currJob->next;
		freeJob(jobForDeletion);
	}
}

void freeJob(Job* job) {
	if (job->pgid != 0)
		killZombies(job);
	freeProcesses(job->headProcess);
	if (job->inPath)
		free(job->inPath);
	if (job->outPath)
		free(job->outPath);
	free(job);
}

void killZombies(Job* job) {
	siginfo_t info;
	int options = WEXITED | WSTOPPED | WCONTINUED | WNOHANG;
	while (1) {
		if (waitid(P_PGID, job->pgid, &info, options) != 0) {
			if (errno != ECHILD)
				perror("Error in bg jobs refining");
			return;
		}
	}
}

void sendSigHups(Job* headJob) {
	for (Job* job = headJob; job != NULL; job = job->next) {
		sigsend(P_PGID, job->pgid, SIGHUP);
		sigsend(P_PGID, job->pgid, SIGCONT);
	}
}

void freeProcesses(Process* headProcess) {
	Process* currProcess = headProcess;
	Process* processForDeletion;
	while (currProcess)
	{
		processForDeletion = currProcess;
		currProcess = currProcess->next;
		int cmdArgIndex = 0;
		while (processForDeletion->cmd.cmdargs[cmdArgIndex]) {
			free(processForDeletion->cmd.cmdargs[cmdArgIndex]);
			cmdArgIndex++;
		}
		wordfree(&processForDeletion->args);
		free(processForDeletion);
	}
}