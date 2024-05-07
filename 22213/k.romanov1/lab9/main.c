#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    pid_t pid;
    int wstatus;
    siginfo_t child_info;

    switch(pid = fork()) {
        case (-1):
            perror("failed to fork");
            exit(1);
        case (0):
            if (execlp("cat", "cat", "largefile", (char*) 0) == -1)
                perror("failed to run cat");

            perror("exec one failed"); 
            exit(1);
        default:
            if (waitpid(pid, &wstatus, WUNTRACED | WCONTINUED) == -1) {
                perror("failed to wait child process");
                exit(1);
            }
            printf("task is completed!\n");
            exit(0);
    }
}

