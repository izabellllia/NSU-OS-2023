#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char * argv[]){
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command> [args]\n", argv[0]);
        return 1;
    }
    
    pid_t pid = fork();
    
    if (pid < 0) {
        perror("failed fork");
        return 1;
    }
    
    if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("failed command");
        return 1;
    }
    
    int status;
    pid_t ret = wait(&status);
    if (ret == -1) {
        perror("failed wait");
        return 1;
    }
    
    if (WIFEXITED(status)) {
        fprintf(stderr, "Child process exited with code: %d\n", WEXITSTATUS(status));
    }

    else if (WIFSIGNALED(status)) {
        fprintf(stderr, "Child process was terminated by signal: %d\n", WTERMSIG(status));
    } else {
        fprintf(stderr, "Child process ended abnormally\n");
        return 1;
    }
    
    return 0;
}
