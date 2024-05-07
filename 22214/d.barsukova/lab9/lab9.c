#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

int main (int argc, char* argv[]) {
    pid_t newPid = fork();
    if (newPid == -1) {
        perror("failure to fork");
        return 1;
    }
    if (newPid == 0) {
        if (execv("/bin/cat", argv) == -1) {
            perror("failure to exec");
            return 1;
        }
    } else {
        if (wait(NULL) == -1) {
            perror("wait error");
            return 1;
        }
        printf("child process is completed\n");
    }
    return 0;
}
