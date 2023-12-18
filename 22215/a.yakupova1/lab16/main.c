#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(){
	int desc = open("/dev/tty", O_RDWR);
	if (desc == -1){
		perror("cannot open file");
		return 1;
	}
	if (isatty(desc) == 0){
		perror("file isn't associated with a terminal");
		return 1;
	}
	struct termios curTerm, saveTerm;

	if (tcgetattr(desc, &curTerm) < 0){
		perror("cannot get attributes");
		return 1;
	}

	saveTerm = curTerm;
	curTerm.c_lflag &= ~ICANON;
	curTerm.c_lflag &= ~ISIG;
	curTerm.c_cc[VMIN] = 1;
	if (tcsetattr(desc, TCSANOW, &curTerm) == -1){
		perror("cannot set attributes");
		return 1;
	}

	printf("Choose any button\n");
	char symb;
	if (read(desc, &symb, 1) == -1){
		perror("cannot read a symbol");
		return 1;
	}
	printf("\nend\n");

	if (tcsetattr(desc, TCSANOW, &saveTerm) == -1){
		perror("cannot set saved terminal's attributes");
		return 1;
	}

	return 0;
}
 
