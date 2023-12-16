#pragma once

#include "shell_structs.h"

Job* createNewJob(Job* headJob);

Process* createNewProcessInJob(Job* job, Command cmd);

void extractJobFromList(Job* job);
Job* getJobByBgNumber(Job* headJob, int bgNumber);

Process* getProcessByPid(Job* job, pid_t pid);

void updateJobStatus(Job* job);
void refineJobStatus(Job* job);
void refineJobsStatuses(Job* headJob);

int isAllProcessesTerminated(Job* job);
int isJobRunning(Job* job);
int isJobDone(Job* job);
int isJobExited(Job* job);
int isJobTerminated(Job* job);
int isJobStopped(Job* job);

void printJobs(Job* headJob);
void printJob(Job* job);
void printProcesses(Job* job);
void printProcess(Process* process);

void printJobsNotifications(Job* headJob, int onlyNotNotified);
void printJobNotification(Job* job);
void printProcessesNotification(Job* job);
void printProcessNotification(Process* process);

void freeJobs(Job* headJob);

void freeProcesses(Process* headProcess);