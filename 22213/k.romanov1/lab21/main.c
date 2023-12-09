#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <curses.h>
#include <unistd.h>

int count = 0; 

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

    while (1) {}
}

void sound_handler() {
    write(STDIN_FILENO, "\a", 1);
    count++;
}

void quit_handler() {
    if (count < 10) {
        char toWrite = count  + '0';
        write(STDOUT_FILENO, &toWrite, sizeof(char));
        exit(0);
    }
    char msg[100];
    int len = 0;

    while (count != 0) {
        char buff = (count % 10) + '0';
        msg[len] = buff;
        len++;
        count /= 10;
    }

    for (int i = len; i >= 0; i--){
        write(STDOUT_FILENO, &msg[i], sizeof(char));
    }

    exit(0);
}
