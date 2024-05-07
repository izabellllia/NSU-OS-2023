#include <unistd.h>
#include <stdio.h>
#include <wait.h>

int main(int argc, char* const argv[])
{
	if (argc < 2)
	{
		printf("Not enough arguments. At least 1 is required.\n");
		return 0;
	}

	pid_t pid_after_fork;

	if ((pid_after_fork = fork()) == -1)
	{
		perror("Can not fork: ");
		return 1;
	}

	if (pid_after_fork == 0)
	{
		printf("Child process execution:\n");
		if (execvp(argv[1], argv + 1) == -1)
		{
			perror("Can not exec: ");
			return 2;
		}
	}


	pid_t wait_return;
	int child_exit_status;

	if ((wait_return = wait(&child_exit_status)) == -1)
	{
		perror("Wait failure: ");
		return 3;
	}

	if (WIFEXITED(child_exit_status) == 0)
	{
		printf("Unexpected child termination (not by exit).\n");
		return 4;
	}
	printf("\n______________________\n\n");
	printf("Child process execution ended.\n");
	printf("Child pid from fork: %d\n", pid_after_fork);
	printf("Wait return: %d\n", wait_return);
	printf("Child exit status: %d\n",WEXITSTATUS(child_exit_status));
	return 0;
}
