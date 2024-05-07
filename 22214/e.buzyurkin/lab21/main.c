#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <curses.h>
#include <unistd.h>

int count = 0;

void handle_sigint(int signum) {
    write(1, "\a", 1);
    count++;
}

void handle_sigquit(int signum) {
    printf("\nbeeped %d times\n", count);
    exit(0);
}


int main() {

    sigset(SIGINT, handle_sigint);

    signal(SIGQUIT, handle_sigquit);

    while (1) {
        
    }

    return 0;
}