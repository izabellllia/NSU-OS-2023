#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    pid_t ret;
    int status;
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error with making new process");
        exit(1);
    }
    if (pid == 0) {
        execlp("cat", "cat", "text.txt", (char *)0);
        perror("Error with execlp");
        exit(1);
    }
    else if (pid > 0) {
        ret = wait(&status);
        if (ret == -1) {
            perror("There is not a single incomplete subprocess");
            exit(1);
        }
        else {
            printf("Child process finished\n");
        }
    }
    return 0;
}
