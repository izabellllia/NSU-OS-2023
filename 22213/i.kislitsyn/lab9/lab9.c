#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <wait.h>

int main()
{
    pid_t pid = fork();
    switch (pid)
    {
    case -1:
        perror("fork");
        exit(EXIT_FAILURE);
        break;
    case 0: //child
        printf("Child process\n");
        execlp("cat", "cat", "1.txt", (char *)NULL);
        perror("exelp");
        exit(EXIT_FAILURE);
        break;
    default:
        if (waitpid(pid, 0, WUNTRACED) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
        printf("Child process ended\n");
        break;
    }
    exit(EXIT_SUCCESS);
}
