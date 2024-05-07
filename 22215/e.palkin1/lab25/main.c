#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MSGSIZE 13

int min (int a, int b)
{
	if (a <= b)
		return a;
	return b;
}

int main()
{
	int fds[2];
	pid_t pid;
	char buffer[MSGSIZE];
	char* message = "Hello World!";
	if (pipe(fds) < 0)
	{
		perror("Error while creating pipe");
		return -1;
	}
	pid = fork();
	if (pid == -1)
	{
		perror("Fork failed");
		return -1;
	}
	else if (pid == 0)
	{
		close(fds[1]);
		int bytes_read = 0;
		while ((bytes_read = read(fds[0], buffer, 5)) > 0)
		{
			for (int i = 0;i < bytes_read;i++)
			{
				printf("%c", toupper(buffer[i]));
			}
		}
		close(fds[0]);
		printf("\n");
	}
	else
	{
		close(fds[0]);
		int bytes_write = 0;
		int offset = 0;
		int message_len = strlen(message);
		int batch_size = 5;
		while (offset < message_len)
		{
			bytes_write = write(fds[1], message + offset, min(batch_size, message_len - offset));
			if (bytes_write < 0)
			{
				perror("Error while writing");
				return -1;
			}
			offset += bytes_write;
		}
		close(fds[1]);
	}
	wait(NULL);
	return 0;
}
