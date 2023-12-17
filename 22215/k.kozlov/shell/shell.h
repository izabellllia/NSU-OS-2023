#pragma once

#include "shell_structs.h"

extern int terminalDescriptor;
extern pid_t shellPgid;
extern struct termios defaultTerminalSettings;
extern int bgFreeNumber;
extern Job* headBgJobFake;
extern char readInterruptionFlag;
extern char prompt[1024];

Job* parseline(char *);
int promptline(char *, char *, int);

void waitFgJob(Job* job);