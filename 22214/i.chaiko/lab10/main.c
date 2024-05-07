#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc == 1) {
        perror("no command");
        return -1;
    }

    pid_t child_pid = fork(), ret;
    int status;

    if (child_pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("execvp error");
        return -2;
    } else if (child_pid > 0) {
        ret = wait(&status);
        if (ret == -1) {
            perror("wait error");
        }
        if (WIFEXITED(status) != 0)
          printf("exit code: %d\n", WEXITSTATUS(status));
    } else {
        perror("fork error");
        return -3;
    }

    return 0;
}
