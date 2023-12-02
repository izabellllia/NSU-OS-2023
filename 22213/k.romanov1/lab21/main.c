#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <curses.h>
#include <unistd.h>

int count = 0; 
int flag = 0;

void sound_handler();
void quit_handler();

int main() {
    struct sigaction beep_sigaction, quit_sigaction;
    beep_sigaction.sa_handler = sound_handler;
    quit_sigaction.sa_handler = quit_handler;

    if (sigaction(SIGINT, &beep_sigaction, NULL) == -1) {
        perror("faield to sigaction beep_handler");
        exit(1);
    }
    if (sigaction(SIGQUIT, &quit_sigaction, NULL) == -1) {
        perror("failed to sigaction quit_handler");
        exit(1);
    }

    while (true) {
        if (flag)
        {
            printf("\nThe beeper worked %d times\n", count);
            exit(0);
        }
    }
}

void sound_handler() {
    write(STDIN_FILENO, "\a", 1);
    count++;
}

void quit_handler() {
    flag = 1;
}
