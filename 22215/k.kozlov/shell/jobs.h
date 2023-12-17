#pragma once

#include "shell_structs.h"

Job* createNewJob(Job* headJob);

Process* createNewProcessInJob(Job* job, Command cmd);

void extractJobFromList(Job* job);
Job* getJobByBgNumber(Job* headJob, int bgNumber);

Process* getProcessByPid(Job* job, pid_t pid);

void updateJobStatus(Job* job);
void updateJobSiginfo(Job* job);
void updateJobsStatuses(Job* headJob);
void cleanJobs(Job* headJob);

int isAllProcessesEnded(Job* job);
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
void freeJob(Job* job);
void sendSigHups(Job* headJob);
void killZombies(Job* job);

void freeProcesses(Process* headProcess);