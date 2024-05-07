#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int execvpe(const char *file, char *const argv[], char *envp[]) {
    char **original_environ = environ;
    environ = envp;
    execvp(file, argv);
    environ = original_environ;
    return -1;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [argumentss]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char *test_envp[] = {"PATH=/home/students/22200/a.kozubenko/NSU-OS-2023/22216/a.kozubenko/lab11/:/usr/bin", NULL};

    if (execvpe(argv[1], &argv[1], test_envp) == -1) {
        perror("execvpe");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

