#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>
#include <errno.h>

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

Process* createNewProcessInJob(Job* job, Command cmd) {
	Process* newProcess = (Process*) malloc(sizeof(Process));
	newProcess->cmd.cmdflag = cmd.cmdflag;
	int cmdArgIndex = 0;
	while (cmd.cmdargs[cmdArgIndex]) {
		newProcess->cmd.cmdargs[cmdArgIndex] = (char*) malloc(strlen(cmd.cmdargs[cmdArgIndex]));
		strcpy(newProcess->cmd.cmdargs[cmdArgIndex], cmd.cmdargs[cmdArgIndex]);
		cmdArgIndex++;
	}
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

void refineJobStatus(Job* job) {
	siginfo_t newStatusInfo;
	int options = WEXITED | WSTOPPED | WCONTINUED | WNOHANG;
	while (1) {
		if (waitid(P_PGID, job->pgid, &newStatusInfo, options) != 0) {
			if (errno != ECHILD)
				perror("Error in bg jobs refining");
			break;
		}
		if (newStatusInfo.si_pid == 0) {
			break;
		}
		Process* p = getProcessByPid(job, newStatusInfo.si_pid);
		p->statusInfo = newStatusInfo;
	}
	updateJobStatus(job);
}

void refineJobsStatuses(Job* headJob) {
	for (Job* job = headJob; job != NULL; job = job->next) {
		refineJobStatus(job);
	}
}

int isAllProcessesTerminated(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code != CLD_EXITED && proc->statusInfo.si_code != CLD_KILLED)
			return 0;
	}
	return 1;
}

// Assume job as running when at least one process is running
int isJobRunning(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code == 0 || proc->statusInfo.si_code == CLD_CONTINUED)
			return 1;
	}
	return 0;
}

// Assume job as done when all processes ended with exit code 0
int isJobDone(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code != CLD_EXITED || proc->statusInfo.si_status != 0)
			return 0;
	}
	return 1;
}

// Assume job as exited when at least one process exited with exit code not 0
int isJobExited(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code == CLD_EXITED && proc->statusInfo.si_status != 0)
			return 1;
	}
	return 0;
}

// Assume job as killed when at least one process was killed by a signal
int isJobTerminated(Job* job) {
	for (Process* proc = job->headProcess; proc != NULL; proc = proc->next) {
		if (proc->statusInfo.si_code == CLD_KILLED)
			return 1;
	}
	return 0;
}

// Assume job as exited when at least one process exited with exit code not 0
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
	for (int argIndex = 0; process->cmd.cmdargs[argIndex]; argIndex++) {
		fprintf(stderr, "\t\tArg %d: %s\n", argIndex, process->cmd.cmdargs[argIndex]);
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
		printProcessNotification(proc);
	}
}

void printProcessNotification(Process* process) {
	fprintf(stdout, "\t");
	for (int argIndex = 0; process->cmd.cmdargs[argIndex]; argIndex++) {
		fprintf(stdout, "%s ", process->cmd.cmdargs[argIndex]);
	}
	fprintf(stdout, "\n");
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
		free(processForDeletion);
	}
}

void freeJobs(Job* headJob) {
	Job* currJob = headJob;
	Job* jobForDeletion;
	while (currJob)
	{
		jobForDeletion = currJob;
		currJob = currJob->next;
		freeProcesses(jobForDeletion->headProcess);
		if (jobForDeletion->inPath)
			free(jobForDeletion->inPath);
		if (jobForDeletion->outPath)
			free(jobForDeletion->outPath);
		free(jobForDeletion);
	}
}