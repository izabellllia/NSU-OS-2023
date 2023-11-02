#include "shell.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <assert.h>
#include <string.h>

#define JOBS_BUFFER_SIZE 128

struct command cmds[MAXCMDS];
char bkgrnd;

void execute_command(int id) {
    if (cmds[id].cmdflag & (OUTFILE | OUTFILEAP)) {
        int out;
        if (cmds[id].cmdflag & OUTFILE)
            out = open(cmds[id].outfile, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        else
            out = open(cmds[id].outfile, O_WRONLY | O_APPEND);

        if (out == -1) {
            perror("Failed to open file");    
            return;
        }
        if (dup2(out, 1) == -1) {
            perror("Failed to redirect output to file");
            return;
        }
    }
    if (cmds[id].cmdflag & INFILE) {
        int in = open(cmds[id].infile, O_RDONLY);
        if (in == -1) {
            perror("Failed to open file");    
            return;
        }
        if (dup2(in, 0) == -1) {
            perror("Failed to redirect output to file");
            return;
        }
    }
    if (execvp(cmds[id].cmdargs[0], cmds[id].cmdargs) == -1) {
        char message[1024] = "Failed to execute command ";
        perror(strcat(message, cmds[id].cmdargs[0]));
        return;
    }
    assert(0);
    return;
}

int main(int argc, char *argv[]) {
    register int i;
    char line[1024];      /*  allow large command lines  */
    int ncmds;
    char prompt[50];      /* shell prompt */

    pid_t jobs[JOBS_BUFFER_SIZE] = {0};
    int next_job = 0;

    /* PLACE SIGNAL CODE HERE */

    sprintf(prompt,"[%s] ", argv[0]);

    while (promptline(prompt, line, sizeof(line)) > 0) {    /* until eof  */
        if ((ncmds = parseline(line)) <= 0) {
            #ifdef DEBUG
            fprintf(stderr, "Unrecognised command\n");
            #endif
            continue;   /* read next line */
        }
        #ifdef DEBUG
        {
            int i, j;
            for (i = 0; i < ncmds; i++) {
                for (j = 0; cmds[i].cmdargs[j] != (char *) NULL; j++)
                    fprintf(stderr, "cmd[%d].cmdargs[%d] = %s\n", i, j, cmds[i].cmdargs[j]);
                fprintf(stderr, "cmds[%d].cmdflag = %o\n", i, cmds[i].cmdflag);
            }
        }
        #endif

        for (i = 0; i < ncmds; i++) {
            pid_t child;
            siginfo_t child_info;
            switch (child = fork()) {
                case 0:
                    execute_command(i);
                    break;
                case -1:
                    perror("Failed to fork");
                    return 1; 
                default:
                    if (bkgrnd) {
                        printf("[%d] %d\n", next_job, child);
                        jobs[next_job] = child;
                        while (jobs[++next_job] != 0) {}
                    } else {
                        if (waitid(P_PID, child, &child_info, WEXITED) == -1) {
                            perror("Failed to wait for child");    
                        }
                    }
            }
        }

        for (int i = 0; i < JOBS_BUFFER_SIZE; i++) {
            if (!jobs[i]) continue;

            siginfo_t info;
            if (waitid(P_PID, jobs[i], &info, WEXITED | WNOHANG) == -1) {
                perror("Failed to wait for a job"); 
                continue;
            }
            if (info.si_pid != 0) {
                printf("[%d] %d Finished\n", i, info.si_pid);
                jobs[i] = 0;
            }
        }

    }/* close while */
}
/* PLACE SIGNAL CODE HERE */
