#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <wait.h>

extern char** environ;

int execvpe(char* file, char* argv[], char* envp[]) {
    char **old_envp = environ;
    environ = envp;
    for (char **p = environ; *p != NULL; p++) {
        printf("%s\n", *p);
    }
    execvp(file, argv);
    environ = old_envp;
    return -1;
}

int main() {
    char *args[] = {"cat", "test.txt", NULL};
    char *envp[] = {"PATH=~/lab11/dir/:/bin/:.", NULL};
    execvpe("cat", args, envp);
    perror("problem with exec");
    return 0;
}
