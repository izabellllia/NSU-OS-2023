#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    int fildes[2];
    if (pipe(fildes) == -1) { 
        return 1; 
    }

    pid_t pid = fork();
    switch(pid){
        case -1: return 1; 

        case 0:
            close(fildes[1]);

            char c;
            while (read(fildes[0], &c, 1) != 0) {
                printf("%c", toupper(c));
            }
            printf("\n");

            close(fildes[0]);
         
        default:
            close(fildes[0]);
            
            char *text = "This is some text\0";
            for (char *ptr = text; *ptr != 0; ptr++) {
                write(fildes[1], ptr, 1);
            }

            close(fildes[1]);
            int status;
            if (waitpid(pid, &status, 0)==-1){
                return 1;
            } 
        
    }
    return 0;

}
