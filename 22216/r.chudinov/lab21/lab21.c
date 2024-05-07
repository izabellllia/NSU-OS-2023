#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE 255

long countOfBeep = 0;
int isContinue = 1;
char str[BUFSIZE];

void sigintHandle(int signal) {
        if(write(1, "\a", 1) == -1) {
                strerror_r(errno, str, BUFSIZE);
                write(2, str, strlen(str));
        } else {
                countOfBeep++;
        }
}

void sigquitHandle(int signal) {
        snprintf(str, BUFSIZE, "Beeped %ld\n", countOfBeep);
        write(1, str, strlen(str));
        isContinue = 0;
}

int main() {
        if(sigset(SIGINT, sigintHandle) == SIG_ERR) {
                perror("SIGINT handler set error");
                return -1;
        }

        if(sigset(SIGQUIT, sigquitHandle) == SIG_ERR) {
                perror("SIGQUIT handler set error");
                return -1;
        }

        while(isContinue) {

        }

        return 0;
}
