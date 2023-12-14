#include <unistd.h>

#define MAXARGS 256
#define MAXCMDS 50

extern char readInterruptionFlag;

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
	int waitStatus;
	struct process* next;
} Process;

typedef struct job {
	Process* headProcess;
	pid_t pgid;
	char fg;
	char stopped;
	int inFd;
	int outFd;
	char* inPath;
	char* outPath;
	char appendFlag;
	int pipesFds[MAXCMDS-1][2];
	struct job* next;
} Job;

Job* createNewJob(Job* headJob);

Process* createNewProcessInJob(Job* job, Command cmd);

void writeJobs(Job* headJob);

void writeProcesses(Job* job);

void freeJobs(Job* headJob);

void freeProcesses(Process* headProcess);

struct job* parseline(char *);
int promptline(char *, char *, int);