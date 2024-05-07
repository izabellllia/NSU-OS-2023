#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv){
	const char* terminal_path = "/dev/tty";
	if (argc > 1){
		terminal_path = argv[1];
		fprintf(stderr, "Terminal changed.");
	}
	int terminal_fd = open(terminal_path, O_RDWR);
	if (terminal_fd == -1){
		fprintf(stderr, "Failed to open file at %s.", terminal_path);
		return -1;
	}
	if (isatty(terminal_fd) == 0) {
		fprintf(stderr, "File at %s is not a terminal.", terminal_path);
		return -1;
	}

	struct termios initial;
	if (tcgetattr(terminal_fd, &initial) == -1){
		perror("Failed to get terminal attributes.");
		return -1;
	}

	struct termios reqired = initial;
	reqired.c_lflag &= ~(ICANON);
	reqired.c_cc[VMIN] = 1;
	if (tcsetattr(terminal_fd, TCSANOW, &reqired) == -1){
		perror("Failed to set terminal attributes.");
		return -1;
	}

	char input;
	char question_writebuf[] = "What's 10 - 9?\n";
	char true_writebuf[] = "\nThis is right answer!\n";
	char false_writebuf[] = "\nWrong answer, try again.\n";
	while (input!='1'){
		write(terminal_fd, question_writebuf, sizeof question_writebuf);
		if(read(terminal_fd, &input, 1) == -1){
			perror("Failed to read symbol.");
			if (tcsetattr(terminal_fd, TCSANOW, &initial) != 0){
				perror("Failed to read symbol. Automatic terminal settings restoration failed.");
				return -1;
			}
			return -1;
		}
		if (input == '1'){
			write(terminal_fd, true_writebuf, sizeof true_writebuf);
		}
		else {
			write(terminal_fd, false_writebuf, sizeof false_writebuf);
		}
	}
	if (tcsetattr(terminal_fd, TCSANOW, &initial) != 0){
		perror("Automatic terminal settings restoration failed.");
		return -1;
	}
	return 0;
}

