#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <curses.h>
#include <unistd.h>


int cnt = 0, quit_flag = 0;


void beep_handler();
void quit();

int main() {
    struct sigaction beep_sigaction, quit_sigaction;
    beep_sigaction.sa_handler = beep_handler;
    quit_sigaction.sa_handler = quit;

    sigaction(SIGINT, &beep_sigaction, NULL);
    sigaction(SIGQUIT, &quit_sigaction, NULL);

    while (1) {
        if (quit_flag)
        {
            printf("\nThe sound was produced %d times\n", cnt);
            exit(EXIT_SUCCESS);
        }
    };
}

void beep_handler() {
    write(1, "\a", 1);
    cnt++;
}

void quit() {
    quit_flag = 1;
}
