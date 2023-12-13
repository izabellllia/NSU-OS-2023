#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
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
    if (count == 0) {
        write(STDOUT_FILENO, "\n0\n", 3);
        exit(0);
    }

    char msg[100];
    int len = 0;
    while (count > 0) {
        msg[len] = count % 10 + '0';
        len++;
        count /= 10;
    }
    msg[len] = '\0';

    write(STDOUT_FILENO, "\n", 1);
    for (int i = len - 1; i >= 0; i--){
        write(STDOUT_FILENO, &msg[i], sizeof(char));
    }
    write(STDOUT_FILENO, "\n", 1);

    exit(0);
}
