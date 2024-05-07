#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/wait.h>

#define TEXT_SIZE 15

void print_upper_case(const char* string)
{
	while (*string != '\0') 
	{
		putchar(toupper(*string++));
	}
}

int main()
{
	int pipe_fd[2];

	if (pipe(pipe_fd) == -1)
	{
		perror("Can not create a pipe");
		return -1;
	}

	pid_t pid;

	if ((pid = fork()) == -1)
	{
                perror("Can not fork");
                return -1;
	}

	else if (pid > 0)
	{
		close(pipe_fd[0]);
		const char output_buffer[TEXT_SIZE] = {"HelLo, woRld!\n"};
		write(pipe_fd[1], output_buffer, TEXT_SIZE);
		wait(0);
	}

	else
	{
		close(pipe_fd[1]);
		char input_buffer[TEXT_SIZE];
		read(pipe_fd[0], input_buffer, TEXT_SIZE);
		print_upper_case(input_buffer);
	}

	return 0;
}
