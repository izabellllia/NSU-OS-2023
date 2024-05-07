#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <wait.h>

#define MSGSIZE 30

int main()
{
    int fd[2];
    int status;
    size_t written = 0;
    char *msgout = "My message";
    char msgin[MSGSIZE];

    if (pipe(fd) == -1)
    {
        perror("Pipe creation failure");
        exit(EXIT_FAILURE);
    }

    pid_t pid;
    switch (pid = fork())
    {
    case -1:
        perror("Fork failure");
        close(fd[0]);
        close(fd[1]);
        exit(EXIT_FAILURE);
    case 0:
        close(fd[1]);

        while ((status = read(fd[0], msgin, MSGSIZE)) > 0)
        {

            for (int i = 0; i < status; i++)
            {
                msgin[i] = toupper(msgin[i]);
            }

            if ((write(STDOUT_FILENO, msgin, status)) == -1)
            {
                perror("Write failure");
                close(fd[0]);
                exit(EXIT_FAILURE);
            }
        }
        printf("\n");

        close(fd[0]);

        if (status == -1)
        {
            perror("Read failure");
            exit(EXIT_FAILURE);
        }

        break;
    default:
        close(fd[0]);

        while (written < strlen(msgout))
        {
            size_t to_send = strlen(&msgout[written]) < MSGSIZE ? strlen(&msgout[written]) : MSGSIZE;
            if ((status = write(fd[1], &msgout[written], to_send)) == -1)
            {
                perror("Write failure");
                close(fd[1]);
                exit(EXIT_FAILURE);
            }
            written += MSGSIZE;
        }
        close(fd[1]);

        if (waitpid(pid, 0, 0) == -1)
        {
            perror("Wait error");
            exit(EXIT_FAILURE);
        }
    }

    exit(EXIT_SUCCESS);
}