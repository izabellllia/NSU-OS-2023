#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		fprintf(stderr, "Provide an editor and a path to file:");
		return -1;
	}
	const char* editor_path = argv[1];
	const char* file_path = argv[2];
	int file_desc = open(file_path, O_RDWR);
	if (file_desc == -1)
	{
		perror("Failed to open file");
		return -1;
	}

	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;
	lock.l_start = 0;
	int fcntl_result = fcntl(file_desc, F_SETLK, &lock);
	if (fcntl_result == -1)
	{
		perror("fcntl failed to lock the file");
		return -1;
	}

	size_t command_length = strlen(editor_path) + 1 + strlen(file_path) + 1;
	char* command = malloc(command_length);
	if (command == NULL)
	{
		perror("Malloc failed");
		return -1;
	}
	sprintf(command, "%s %s", editor_path, file_path);

	int command_result = system(command);
	if(command_result == -1)
	{
		fprintf(stderr, "Execution of command '%s' failed.", command);
		perror(NULL);
		return -1;
	}
	else if(WIFEXITED(command_result) && WEXITSTATUS(command_result) == 127)
	{
		fprintf(stderr, "Fail to execute 'shell' in the child process.");
		return -1;
	}
	
	lock.l_type = F_UNLCK;
	fcntl_result = fcntl(file_desc, F_SETLK, &lock);
	if(fcntl_result == -1)
	{
		perror("fcntl failed to unlock the file");
		return -1;
	}
	free(command);

	return 0;
}
