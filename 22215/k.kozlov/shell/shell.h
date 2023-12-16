#include <unistd.h>
#include <sys/wait.h>

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
	siginfo_t statusInfo;
	struct process* prev;
	struct process* next;
} Process;

typedef struct job {
	Process* headProcess;
	pid_t pgid;
	char initialFg;
	int bgNumber;
	char stopped;
	int inFd;
	int outFd;
	char* inPath;
	char* outPath;
	char appendFlag;
	int pipesFds[MAXCMDS-1][2];
	struct job* prev;
	struct job* next;
} Job;

Job* createNewJob(Job* headJob);

Process* createNewProcessInJob(Job* job, Command cmd);

void extractJobFromList(Job* job);
Job* getJobByBgNumber(Job* headJob, int bgNumber);

Process* getProcessByPid(Job* job, pid_t pid);

int isAllProcessesTerminated(Job* job);

void printJobs(Job* headJob);
void printJob(Job* job);

void printProcesses(Job* job);
void printProcess(Process* process);

void freeJobs(Job* headJob);

void freeProcesses(Process* headProcess);

struct job* parseline(char *);
int promptline(char *, char *, int);