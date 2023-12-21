#include <wait.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
    
int main(){
    int ifError;
    int child_status;
    pid_t ifError0;
    printf("\nDoing shit\n\n");
    pid_t pid_1 = fork();
 
    switch(pid_1){
        case(-1):
            exit(EXIT_FAILURE);
            break;
        case(0):
            ifError = execlp("cat","cat","1.txt",(char *)0);
            if(ifError < 0)
                exit(EXIT_FAILURE);
            break;
        default:
            ifError0 = waitpid(pid_1, &child_status, WUNTRACED);
            if(ifError0 < 0)
                exit(EXIT_FAILURE);
            printf("\n\nDone\n");
            break;
    }
    exit(EXIT_SUCCESS);
}
