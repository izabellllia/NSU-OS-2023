#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char* argv[]) {
    int status;
    pid_t procID;

    if ((procID = fork()) == -1) {
        perror("fork() failed ");
        exit(EXIT_FAILURE);
    }

    switch (procID)
    {
    case 0:
        printf("Child process:\n");
        execl("/bin/cat", "cat", argv[1], (char*) 0);
        perror("execl() failed ");
        exit(EXIT_FAILURE);
        break;
    
    default:
        if (wait(&status) == -1) {
            perror("wait() failed ");
            exit(EXIT_FAILURE);
        }
        printf("some text by parent process\n");
        break;
    }

    exit(EXIT_SUCCESS);
}