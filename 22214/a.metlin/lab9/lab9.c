#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    pid_t pid = fork();
    
    if(pid == -1){
        perror("error in fork()");
        return 1;
    } else if(pid == 0){
        execlp("cat", "cat", "lab9.c", NULL);
        perror("execl() error");
        return 1;
    } else{
        wait(NULL);
        printf("\nHello\n");
    }
    return 0;
}