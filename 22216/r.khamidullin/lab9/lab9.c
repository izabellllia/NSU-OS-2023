#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file name>\n", argv[0]);
        return -1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("Failed to fork");
        return -1;
    }

    if (pid == 0) {
        execl("/bin/cat", "cat", argv[1], (char *) 0);
        perror("Failed to call cat");
        exit(-1);
    }

    int status;
    wait(&status);

    if (!WIFEXITED(status)) {
        fprintf(stderr, "Fork ended not by exit.\n");
    }

    fprintf(stdout, "Parent finished.\n");
    return 0;
}
