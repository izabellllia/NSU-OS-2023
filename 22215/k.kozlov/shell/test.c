#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <time.h>

int main() {
	struct timespec delay;
	delay.tv_sec = 5;
	delay.tv_nsec = 1000000000;
	sleep(10);
	fprintf(stdout, "Time is out\n");
	return 0;
}
