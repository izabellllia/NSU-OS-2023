#include <unistd.h>

#define MAXARGS 256
#define MAXCMDS 50

typedef struct command {
	char *cmdargs[MAXARGS];
	char cmdflag;
} Command;

/*  cmdflag's  */
#define OUTPIP  01
#define INPIP   02

extern struct command cmds[];
extern char *infile, *outfile, *appfile;
extern char bkgrnd;

int parseline(char *);
int promptline(char *, char *, int);

typedef struct process {
	Command cmd;
	pid_t pid;
	int waitStatus;
	struct process* next;
} Process;

typedef struct job {
	Process* headProcess;
	pid_t pgid;
	char stopped;
	int pipesFds[MAXCMDS-1][2];
	struct job* next;
} Job;