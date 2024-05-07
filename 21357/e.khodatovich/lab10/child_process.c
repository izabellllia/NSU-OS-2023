#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>



void child_process (int argc, char** args)
{
	if (argc < 2)
	{
		fprintf(stderr, "No arguments\n");
		exit(EXIT_FAILURE);
	}
	if (execvp(args[1], args+1))
	{
		perror("execl");
		exit(EXIT_FAILURE);
	}	
}

int main (int argc, char** args)
{
	pid_t child_pid;

	child_pid = fork();
	int wstatus;
	pid_t w;
	switch (child_pid)
	{
		case -1:
			perror ("fork");
			exit (EXIT_FAILURE);
		case 0:
			child_process(argc, args);
		default:
			w = waitpid(child_pid, &wstatus, WUNTRACED);
			if (w == -1)
			{
				perror ("waitpid");
				exit (EXIT_FAILURE);
			}	

			if (WIFEXITED(wstatus))
			{
				printf ("child finished with exit code %d\n", WEXITSTATUS(wstatus));
			}
			else if (WIFSIGNALED(wstatus))
			{
				printf ("child is terminated by signal %d\n", WTERMSIG(wstatus));
			}
			else if (WIFSTOPPED(wstatus))
			{
				printf ("child is stopped by signal %d\n", WSTOPSIG(wstatus)); 
			}
			else
			{
				fprintf(stderr, "child status is unknown\n");
				exit(EXIT_FAILURE);
			}
			exit (EXIT_SUCCESS);
	}
}
