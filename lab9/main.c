#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>



int main(int argc, int *argv[]){
        if (argc < 2)
        {
                fprintf(stderr, "Error: -no_file_name.\n", argv[0]);
                return 1;
        }

        pid_t child_pid = fork();

        if(child_pid == -1){
                perror("Error: -fork()");
                return 1;
        }
        if(child_pid == 0){
                execl("/bin/cat", "cat", argv[1], NULL);
                perror("Error: -execl()");
                return 1;
        }

        pid_t end_pid = wait(0);
        if(end_pid == -1){
                perror("Error: -child_process");
                return 1;
        }
        printf("You back in parent process.\n");
        return 0;
}



