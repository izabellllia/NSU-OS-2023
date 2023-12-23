#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <curses.h>

int cn = 0;

void sigint_handler(){
    write(1, "\a", 1);
    cn+=1;
}

void sigquit_handler(){
    printf("\nbeep times: %d\n", cn);
    exit(0);
}

int main(){

    sigset(SIGINT, sigint_handler);

    signal(SIGQUIT, sigquit_handler);

    while(1){

    }

    return 0;
}