#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#include "shell.h"
#include "shell_structs.h"
#include "jobs.h"
#include "shell_cmds.h"

int processShellSpecificMainCommand(Job* job) {
	Process* process = job->headProcess;
	if (process->next || !process->cmd.cmdargs[1]) {
		return -1;
	}
	if (process->cmd.cmdargs[1][0] == '%') {
		if (strcmp(process->cmd.cmdargs[0], "fg") == 0) {
			fg_cmd(process->cmd.cmdargs[1]);
			return 0;
		}
		if (strcmp(process->cmd.cmdargs[0], "bg") == 0) {
			bg_cmd(process->cmd.cmdargs[1]);
			return 0;
		}
		if (strcmp(process->cmd.cmdargs[0], "kill") == 0) {
			kill_cmd(process->cmd.cmdargs[1]);
			return 0;
		}
	}
	if (strcmp(process->cmd.cmdargs[0], "cd") == 0) {
		cd_cmd(process->cmd.cmdargs[1]);
		return 0;
	}
	return -1;
}

int processShellSpecificForkedCommand(Process* process) {
	if (strcmp(process->cmd.cmdargs[0], "jobs") == 0) {
		jobs_cmd();
		return 0;
	}
	return -1;
}

void jobs_cmd() {
	// TODO: Реализовать более человеческий метод вывода
	printJobs(headBgJobFake->next);
}

static int parseFromIntFromPercentArg(char* arg) {
	int num;
	sscanf(arg+1, "%d", &num);
	return num;
}

void fg_cmd(char* argNum) {
	int bgNumber = parseFromIntFromPercentArg(argNum);
	Job* bgJob = getJobByBgNumber(headBgJobFake, bgNumber);
	if (!bgJob)	{
		fprintf(stderr, "Background job %d wasn't found\n", bgNumber);
		return;
	}
	if (lastBgJob == bgJob)
		lastBgJob = bgJob->prev;
	extractJobFromList(bgJob);
	tcsetpgrp(terminalDescriptor, bgJob->pgid);
	sigsend(P_PGID, bgJob->pgid, SIGCONT);
	waitFgJob(bgJob);
	tcsetpgrp(terminalDescriptor, shellPgid);
	tcsetattr(terminalDescriptor, TCSAFLUSH, &defaultTerminalSettings);
}

void bg_cmd(char* argNum) {
	int bgNumber = parseFromIntFromPercentArg(argNum);
	Job* bgJob = getJobByBgNumber(headBgJobFake, bgNumber);
	if (!bgJob)	{
		fprintf(stderr, "Background job %d wasn't found\n", bgNumber);
		return;
	}
	sigsend(P_PGID, bgJob->pgid, SIGCONT);
}

void kill_cmd(char* argNum) {
	int bgNumber = parseFromIntFromPercentArg(argNum);
	Job* bgJob = getJobByBgNumber(headBgJobFake, bgNumber);
	if (!bgJob)	{
		fprintf(stderr, "Background job %d wasn't found\n", bgNumber);
		return;
	}
	sigsend(P_PGID, bgJob->pgid, SIGINT);
}

void cd_cmd(char* path) {
	if (strcmp(path, "~") == 0) {
		path = getenv("HOME");
	}
	if (chdir(path) != 0) {
		perror("Failed to changed directory");
		return;
	}
}