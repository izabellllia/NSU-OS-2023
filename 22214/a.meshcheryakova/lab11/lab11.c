#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>
extern char **environ;

int execvpe(char* filename, char* argv[], char* envp[]) {
    environ = envp;
    execvp(filename, argv);
    return -1;
}

int main() {
    pid_t ret;
    int status;
    pid_t pid = fork();
    if (pid == -1) {
        perror("Error with making new process");
        exit(1);
    }
    if (pid == 0) {
        char* argv[] = {"test", "a", "b", "c", NULL};
        char* envp[] = {"NAME=value", "nextname=nextvalue", "HOME=/xyz", "PATH=/home/students/22200/a.meshcheryakova", NULL};
        execvpe("test", argv, envp);
        perror("Error with execvpe\n");
        exit(1);
    }
    else {
        ret = wait(&status);
        if (ret == -1) {
            perror("There is not a single incomplete subprocess");
            exit(1);
        }
        else{
            printf("Child process finished\n");
        }
    }
    exit(0);
}
