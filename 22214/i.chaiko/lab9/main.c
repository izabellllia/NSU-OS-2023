#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t child_pid = fork(), ret;
    int status;

    if (child_pid == 0) {
        execlp("cat", "cat", "longFile", NULL);
        perror("execl error");
        return -1;
    } else if (child_pid > 0) {
        ret = wait(&status);
        printf("\nparent process finished\n");
    } else {
        perror("fork error");
        return -1;
    }

    return 0;
}