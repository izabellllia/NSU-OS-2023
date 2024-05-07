#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    pid_t pid;
    int status;

    switch (pid = fork()) {
    case -1:
        perror("fork() error");
        exit(1);
    case 0: // child 
        execlp("cat", "cat", "lab9.c", NULL);
        perror("execl() error");
        exit(7);
        break;
    default: //current (parent)
        wait(NULL);
        printf("Hello,  world\n");
    }
    return 0;
}

