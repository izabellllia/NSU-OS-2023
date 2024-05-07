#include<stdio.h>
#include<termios.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>

int main() {
	
	struct termios new_settings;
	struct termios stored_settings;
	
	if (tcgetattr(0, &stored_settings) != 0) {
		perror("Error while getting terminal settings");
		exit(errno);
	}
	
	new_settings = stored_settings;
	
	new_settings.c_lflag &= (~ICANON);
	new_settings.c_cc[VTIME] = 0;
	new_settings.c_cc[VMIN] = 1; 
	
	
	if (tcsetattr(0, TCSANOW, &new_settings) != 0) {
		perror("Error while updating terminal settings");
		exit(errno);
	}

	printf("?\n");
	char x;
	read (0, &x, 1);
		
	if (tcsetattr(0, TCSANOW, &stored_settings) != 0) {
		perror("Error while returning old terminal settings");
		exit(errno);
	}

	return 0;
}
