#pragma once

#include <unistd.h>
#include <sys/wait.h>

#define MAXARGS 256
#define MAXCMDS 50

typedef struct command {
	char *cmdargs[MAXARGS];
	char cmdflag;
} Command;

/*  cmdflag's  */
#define OUTPIP  01
#define INPIP   02

typedef struct process
{
	Command cmd;
	pid_t pid;
	siginfo_t statusInfo;
	struct process* prev;
	struct process* next;
} Process;

typedef struct job {
	Process* headProcess;
	pid_t pgid;
	char initialFg;
	int bgNumber;
	char status;
	char notified;
	int inFd;
	int outFd;
	char* inPath;
	char* outPath;
	char appendFlag;
	struct job* prev;
	struct job* next;
} Job;

#define J_SETTING 0
#define J_RUNNING 1
#define J_DONE 2
#define J_EXIT 3
#define J_TERMINATED 4
#define J_STOPPED 5