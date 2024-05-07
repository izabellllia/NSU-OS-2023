#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int counter = 0;

void in_signal_hendler(int sign) {
    printf("\a\nsignal received\n");
    counter++;
}

void out_signal_hendler(int sign) {
    printf("\n num of signals is %d\n", counter);
    exit(0);
}

int main() {
    sigset(SIGINT, in_signal_hendler);
    sigset(SIGQUIT, out_signal_hendler);

    while(1) {

    }

    return 0;
}

