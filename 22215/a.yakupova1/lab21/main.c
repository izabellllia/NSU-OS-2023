#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int total = 0;
int end = 0;

void inter_handler() {
	write(STDOUT_FILENO, "\007", 1);
	total += 1;
}

void quit_handler() {
	end = 1;
}

int main() {
    struct sigaction inter, quit;
    inter.sa_handler = inter_handler;
    quit.sa_handler = quit_handler;

    sigaction(SIGINT, &inter, NULL);
    sigaction(SIGQUIT, &quit, NULL);

    while(1){
	if (end){
	    printf("\nTotal: %d\n", total);
	    return 1;
	}
    }
}
