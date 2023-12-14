#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "shell.h"

Job* createNewJob(Job* headJob) {
	Job* newJob = (Job*) malloc(sizeof(Job));
	newJob->headProcess = NULL;
	newJob->pgid = 0;
	newJob->fg = 1;
	newJob->inFd = STDIN_FILENO;
	newJob->outFd = STDOUT_FILENO;
	newJob->inPath = NULL;
	newJob->outPath = NULL;
	newJob->appendFlag = 0;
	newJob->stopped = 0;
	
	if (headJob == NULL)
		return newJob;
	Job* currentJob = headJob;
	while (currentJob->next != NULL) {
		currentJob = currentJob->next;
	}

	currentJob->next = newJob;
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
	newProcess->waitStatus = 0;
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
	return newProcess;
}

void writeJobs(Job* headJob) {
	for (Job* currentJob = headJob; currentJob != NULL; currentJob = currentJob->next) {
		fprintf(stderr, "Job with group ID %d, IN %d(%s), OUT %d(%s%s), FG %d:\n", 
			currentJob->pgid, currentJob->inFd, currentJob->inPath, 
			currentJob->outFd, currentJob->appendFlag ? "+ " : "", currentJob->outPath, currentJob->fg);
		writeProcesses(currentJob);
	}
}

void writeProcesses(Job* job) {
	Process* currentProcess = job->headProcess;
	while (currentProcess != NULL) {
		fprintf(stderr, "\tProcess %d, PIPES %d\n", currentProcess->pid, currentProcess->cmd.cmdflag);
		int argIndex = 0;
		while (currentProcess->cmd.cmdargs[argIndex])
		{
			fprintf(stderr, "\t\tArg %d: %s\n", argIndex, currentProcess->cmd.cmdargs[argIndex]);
			++argIndex;
		}
		
		currentProcess = currentProcess->next;
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