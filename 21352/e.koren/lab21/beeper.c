#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

static int signalCount = 0;
static bool stopBeeper = false;

void sigCatchh(int sig){
	if(sig == SIGINT){
		write(STDIN_FILENO,"\a\n",2);
		signalCount++;
		//sleep(1);
	}
	if(sig == SIGQUIT){
		stopBeeper = true;
	}		
}

void main(){
	struct sigaction act= {
		.sa_handler = 0,
		.sa_sigaction = 0,
		.sa_mask = 0,
		.sa_flags = 0};
	act.sa_flags = SA_NODEFER;
	act.sa_handler = sigCatchh;
	if (sigaction(SIGINT,&act,NULL)==-1){
		perror("sigaction");
		exit(1);
	}
	if (sigaction(SIGQUIT,&act,NULL)==-1){
		perror("sigaction");
		exit(1);
	}
	for(;;){
		pause();
		if (stopBeeper){	
			printf("beeper was called %d times\n",signalCount);
			exit(0);
		}
	} 

}
