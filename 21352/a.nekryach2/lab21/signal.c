#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int count = 0;
void sigCatch(int sig)
{
    signal(sig, SIG_IGN);
    if (sig == SIGINT)
    {
         write(STDOUT_FILENO, "\a", sizeof(char));
         count++;
    }
    if (sig == SIGQUIT)
    {
         char buf[40];
         sprintf(buf,"SIGINT amount: %d\n", count);
         write(STDOUT_FILENO,buf,strlen(buf));
         exit(1);
    }
    signal(sig, sigCatch);
}

void main()
{
    signal(SIGINT, sigCatch);
    signal(SIGQUIT, sigCatch);

    while(1)
    {
        pause();
    }
}
