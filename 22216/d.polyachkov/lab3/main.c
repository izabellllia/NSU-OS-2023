#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
	int euid = geteuid();
	printf("RID: %d\nEUID: %d\n", getuid(), euid);
	FILE* file = fopen("some_file", "r");
	if(file != NULL)
	{
		fclose(file);
	}
	else
	{
		perror("Error in function main. Error read file with name \"some_file\" ");
	}

	if (setuid(getuid()) != 0)
	{
		perror("Error in function main. Error to set new EUID");
		exit(-1);
	}

	printf("RID: %d\nEUID: %d\n", getuid(), geteuid());
	file = fopen("some_file", "r");
	if(file != NULL)
	{
		fclose(file);
	}
	else
	{
		perror("Error in function main. Error read file with name \"some_file\" ");
	}

	exit(0);
}
