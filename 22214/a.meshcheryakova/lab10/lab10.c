#include <unistd.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    pid_t ret;
    int status;
    if (argc == 1) {
        perror("No command to execute");
        exit(1);
    }
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error with making new process");
        exit(1);
    }
    if (pid == 0) {
        execvp(argv[1], argv + 1);
        perror("Error with execvp");
        exit(1);
    }
    else {
        ret = wait(&status);
        if (ret == -1) {
            perror("There is not a single incomplete subprocess");
        }
        else {
            if (WIFEXITED(status)) {
                printf("child's exit status is: %d\n", WEXITSTATUS(status));
            }
            else {
                printf("child's process was ended with signal");
            }
        }
    }
    exit(0);
}
