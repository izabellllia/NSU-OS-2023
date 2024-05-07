#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args]\n", argv[0]);
        return -1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Failed to fork");
        return -1;
    }

    if (pid == 0) {
        execvp(argv[1], argv + 1);
        perror("Failed to exec");
        exit(-1);
    }

    int status;
    wait(&status);

    if (!WIFEXITED(status)) {
        fprintf(stderr, "Fork ended not by exit.\n");
    } else {
        fprintf(stdout, "Fork ended with the code %d.\n", WEXITSTATUS(status));
    }
    return 0;
}
