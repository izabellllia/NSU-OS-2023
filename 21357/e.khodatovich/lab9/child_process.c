#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

char separator[21] = "====================\n";


void print_file(char * const name)
{
    int err = execl("/usr/bin/cat","cat",name, (char*) 0);
    if (err  == -1);
    {
        perror("execl");
    }
}

void child_process (int argc, char** args)
{
    if (argc < 2)
    {
        fprintf(stderr, "No arguments\n");
        return;
    }    
    if (argc > 2)
    {
        fprintf(stderr, "Too many argumets\n");
        return;
    }
    print_file(args[1]);
}

int main (int argc, char** args)
{
    pid_t child_pid;

    printf("Attempt to create child process\n");
    child_pid = fork();
    int wstatus;
    pid_t w;
    switch (child_pid)
    {
        case -1:
            perror ("fork");
            exit (EXIT_FAILURE);
        case 0:
            printf("Start of child process\n%s", separator);
            child_process(argc, args);
            exit (EXIT_FAILURE);
        default:
            w = waitpid(child_pid, &wstatus, WUNTRACED);
            if (w == -1)
            {
                perror ("waitpid");
                exit (EXIT_FAILURE);
            }	
            printf ("%sContinuation of parent process \n", separator);
            exit (EXIT_SUCCESS);
    }
}
