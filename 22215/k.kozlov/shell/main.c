#include <termios.h>
#include <stdio.h>

#include "shell.h"
#include "shell_structs.h"
#include "jobs.h"
#include "shell_cmds.h"

int terminalDescriptor;
pid_t shellPgid;
struct termios defaultTerminalSettings;
int bgFreeNumber = 1;
Job* headBgJobFake = NULL;
char readInterruptionFlag = 0;
char prompt[1024];

int main(int argc, char *argv[]) {
	char line[1024];
	Job* newJobsHead;
	Job* currentJob;
	Job* nextJob;

	initShell();

	while (promptline(updatePrompt(), line, sizeof(line)) > 0) {
		if (readInterruptionFlag) {
			fprintf(stderr, "\n");
			readInterruptionFlag = 0;
			continue;
		}
		if ((newJobsHead = parseline(line)) == NULL)
			continue;
		nextJob = newJobsHead;
		while (nextJob != NULL) {
			updateJobsStatuses(headBgJobFake->next);

			currentJob = nextJob;
			nextJob = nextJob->next;
			extractJobFromList(currentJob);
			if (!processShellSpecificMainCommand(currentJob))
				processJob(currentJob);
			
			updateJobsStatuses(headBgJobFake->next);
			printJobsNotifications(headBgJobFake->next, 1);
			cleanJobs(headBgJobFake->next);
		}
	}
	if (headBgJobFake->next) {
		fprintf(stderr, "\nThese background jobs will get SIGHUP:\n");
		sendSigHups(headBgJobFake->next);
		updateJobsStatuses(headBgJobFake->next);
		printJobsNotifications(headBgJobFake->next, 0);
	}
	fprintf(stderr, "Bye!\n");
	freeJobs(headBgJobFake);
	return 0;
}