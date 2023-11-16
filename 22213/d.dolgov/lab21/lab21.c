#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

static int counter = 0;

void interruptHandler(int sig) {
    char *message = "\a";
    write(STDIN_FILENO, message, sizeof(*message));
    counter++;
    signal(sig, SIG_IGN);
    signal(SIGINT, interruptHandler);
}

void quitHandler(int sig) {
    if (counter < 10) {
        char buff = counter  + '0';
        write(STDIN_FILENO, &buff, sizeof(counter));
        exit(0);
    }
    char message[100];
    int counter_ = 0;

    while (counter != 0) {
        char buff = (counter % 10) + '0';
        message[counter_] = buff;
        counter_++;
        counter /= 10;
    }

    for (int i = counter_; i >= 0; i--){
        write(STDIN_FILENO, &message[i], sizeof(char));
    }

    exit(0);
}

int main(void) {
    signal(SIGINT, interruptHandler);
    signal(SIGQUIT, quitHandler);

    while (1) {}

    exit(0);
}
