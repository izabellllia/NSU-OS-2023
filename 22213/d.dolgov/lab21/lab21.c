#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

static int counter = 0;

void interruptHandler(int sig) {
    printf("\a\n");
    counter++;
    signal(sig, SIG_IGN);
    signal(SIGINT, interruptHandler);
}

void quitHandler(int sig) {
    printf("\nALARM FIRED %d TIMES\n", counter);
    exit(0);
}

int main(void) {
    signal(SIGINT, interruptHandler);
    signal(SIGQUIT, quitHandler);

    while (1) {}

    exit(0);
}
