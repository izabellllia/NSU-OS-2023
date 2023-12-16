#pragma once

#include "shell_structs.h"

int processShellSpecificForkedCommand(Process* process);
void jobs_cmd();

// Only one process in job!
int processShellSpecificMainCommand(Job* job);
void fg_cmd(char* argNum);
void bg_cmd(char* argNum);
void kill_cmd(char* argNum);
void cd_cmd(char* path);