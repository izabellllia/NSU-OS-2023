#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern char **environ;

int execvpe(const char* path, char *const argv[], char *const envp[]) {
    environ = envp;
    execvp(path, argv);
    return -1;
}

int main() {
    char *argv[] = {"date", NULL};
    char *new_environ[] = {"TZ=America/Los_Angeles", NULL};

    execvpe(argv[0], argv, new_environ);
    perror("exec error");
    return 0;
}
