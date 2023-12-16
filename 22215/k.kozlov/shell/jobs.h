#pragma once

#include "shell_structs.h"

Job* createNewJob(Job* headJob);

Process* createNewProcessInJob(Job* job, Command cmd);

void extractJobFromList(Job* job);
Job* getJobByBgNumber(Job* headJob, int bgNumber);
void refineJobStatus(Job* job);
void refineJobsStatuses(Job* headJob);

Process* getProcessByPid(Job* job, pid_t pid);

int isAllProcessesTerminated(Job* job);

void printJobs(Job* headJob);
void printJob(Job* job);

void printProcesses(Job* job);
void printProcess(Process* process);

void freeJobs(Job* headJob);

void freeProcesses(Process* headProcess);