#include <wait.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(){
    printf("\nDoing shit\n\n");
    pid_t pid_1 = fork();
    if(pid_1 < 0){
        exit(EXIT_FAILURE);
    }
    if(pid_1 == 0){
        int ifError = execl("/bin/cat","cat","1.txt",(char *)0);
        if(ifError < 0){
            exit(EXIT_FAILURE);
        }
    }
    if(pid_1 > 0){
        pid_t ifError0 = wait(0);
        if(ifError0 < 0){
            exit(EXIT_FAILURE);
        }
        printf("\nDone\n");
    }
    exit(EXIT_SUCCESS);
}
