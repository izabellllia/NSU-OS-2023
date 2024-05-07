#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

int main()
{
    int status;
    printf("Parent pid: %d\n", getpid());
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("Failed to create a copy of the process");
        return -1;
    }
    if (pid == 0)
    {
        printf("Child pid: %d\n", getpid());
        execlp("cat", "cat", "test.txt", (char *) NULL);
        perror("Faild to execute cat for test.txt file");
	return -1;
    }
    else
    {
        wait(&status);
        printf("Some text from parrent process\n");
        return 0;
    }
}
