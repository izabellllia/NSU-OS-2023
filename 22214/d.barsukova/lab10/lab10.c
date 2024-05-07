#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

int main (int argc, char* argv[]) {
    int status;
    pid_t newPid;
    if ((newPid = fork()) == -1) {
        perror("failure to fork");
        return 1;
    }
    if (newPid == 0) {
        if (execvp(argv[1], &argv[1]) == -1) {
            perror("failure to exec");
            return 1;
        }
    }
    if (wait(&status) == -1) {
        perror("wait error");
        return 1;
    }
    if (WIFEXITED(status) != 0) {
        printf("Exit status: %d\n", WEXITSTATUS(status));
    } else {
        perror("exit error");
        return 1;
    }
    return 0;
}
