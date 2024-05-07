#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>

int main(int argc, char** argv)
{
	const char* path_to_terminal = "/dev/tty";
	if (argc < 2)
	{
		fprintf(stderr, "Path to terminal was not passed. %s is used by default.\n", path_to_terminal);
	}
	else
	{
		path_to_terminal = argv[1];
	}

	const int terminal_fd = open(path_to_terminal, O_RDWR);
	if (terminal_fd == -1)
	{
		perror("open terminal");
		return -1;
	}

	if (isatty(terminal_fd) == 0)
	{
		fprintf(stderr, "File %s is not a terminal.\n", path_to_terminal);
		return -1;
	}

	struct termios tty, savtty;
	if (tcgetattr(terminal_fd, &tty) == -1)
	{
		perror("Get terminal attributes");
		return -1;
	}
	savtty = tty;
	tty.c_lflag &= ~ (ICANON | ISIG);
	tty.c_cc[VMIN] = 1;
	if (tcsetattr(terminal_fd, TCSANOW, &tty) == -1)
	{
		perror("Set terminal attributes");
		return -1;
	}

	printf("Check the terminal.\n");
	const char request_msg[] = "Guess my name:\n\ta. Nikita\n\tb. Prokhor\n\tc. Antip\n\td. Svyatozar\n\nPrint the letter of the answer: ";
	const char correct_answer = 'a';
	if (write(terminal_fd, request_msg, sizeof(request_msg) - 1) == -1)
	{
		perror("write to terminal");
		return -1;
	}

	char answer;
	if (read(terminal_fd, &answer, 1) == -1)
	{
		perror("read from terminal");
		return -1;
	}

	const char answer_received_msg[] = "\nAnswer was received. Check the result in stdout.\n";
	if (write(terminal_fd, answer_received_msg, sizeof(answer_received_msg) - 1) == -1)
	{
		perror("write to terminal");
		return -1;
	}

	if (answer == correct_answer)
	{
		printf("Correct!!!!!!!!!!!!!!!!!!!!!\n");
	}
	else
	{
		printf("Incorrect ((((((((((((((((((((\n");
	}

	if (tcsetattr(terminal_fd, TCSANOW, &savtty) == -1)
	{
		perror("Restore initial terminal attributes");
		return -1;
	}

	return 0;
}
