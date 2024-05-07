#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#define MAX_COUNTER_LENGTH 20
#define COUNTER_PREFIX_LENGTH 19
#define COUNTER_MSG_LENGTH (COUNTER_PREFIX_LENGTH + MAX_COUNTER_LENGTH)

static int counter = 0;

void sigcatch(int sig)
{
    if (sig == SIGINT)
    {
        if (counter == INT_MAX)
        {
            write(STDOUT_FILENO, "Max counter was reached\n", 24);

            char buf[COUNTER_MSG_LENGTH];
            sprintf(buf, "\nbells counter = %d\n", counter);
            write(STDOUT_FILENO, buf, strlen(buf));

            exit(-1);
        }

        write(STDOUT_FILENO, "\a\n", 2);
        ++counter;
    }

    else if (sig == SIGQUIT)
    {
        char buf[COUNTER_MSG_LENGTH];
        sprintf(buf, "\nbells counter = %d\n", counter);
        write(STDOUT_FILENO, buf, strlen(buf));
        exit(-1);
    }
}

int main(int argc, char** argv)
{
    if (sigset(SIGINT, sigcatch) == SIG_ERR)
    {
        perror("Can not set SIGINT handler");
        exit(-1);
    }

    if (sigset(SIGQUIT, sigcatch) == SIG_ERR)
    {
        perror("Can not set SIGQUIT handler");
        exit(-1);
    }

    while(1)
    {
        pause();
    }
}
