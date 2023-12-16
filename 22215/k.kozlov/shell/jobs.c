#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>
#include <sys/wait.h>

#include "shell.h"

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
	newJob->stopped = 0;
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
	for (Job* currentJob = headJob; currentJob != NULL; currentJob = currentJob->next) {
		if (currentJob->bgNumber == bgNumber)
			return currentJob;
	}
	return NULL;
}

Process* getProcessByPid(Job* job, pid_t pid) {
	Process* currentProcess = job->headProcess;
	while (currentProcess != NULL) {
		if (currentProcess->pid == pid)
			return currentProcess;
		currentProcess = currentProcess->next;
	}
	return NULL;
}

int isAllProcessesTerminated(Job* job) {
	Process* currentProcess = job->headProcess;
	while (currentProcess != NULL) {
		if (currentProcess->statusInfo.si_code != CLD_EXITED 
			&& currentProcess->statusInfo.si_code != CLD_KILLED)
			return 0;
		currentProcess = currentProcess->next;
	}
	return 1;
}

void printJobs(Job* headJob) {
	for (Job* currentJob = headJob; currentJob != NULL; currentJob = currentJob->next) {
		printJob(currentJob);
		printProcesses(currentJob);
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
	Process* currentProcess = job->headProcess;
	while (currentProcess != NULL) {
		printProcess(currentProcess);
		currentProcess = currentProcess->next;
	}
}

void printProcess(Process* process) {
	fprintf(stderr, "\tProcess %d, PIPES %d\n", process->pid, process->cmd.cmdflag);
	fprintf(stderr, "\tCode %d\n\tStatus %d\n", 
		process->statusInfo.si_code, process->statusInfo.si_status);
	int argIndex = 0;
	while (process->cmd.cmdargs[argIndex])
	{
		fprintf(stderr, "\t\tArg %d: %s\n", argIndex, process->cmd.cmdargs[argIndex]);
		++argIndex;
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