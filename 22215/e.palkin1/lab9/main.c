#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s fileName\n", argv[0]);
		return 1;
	}

	pid_t process_id = fork();
	if (process_id == -1){
		perror("Fork error");
		return 1;
	}
	if (process_id == 0){
		execl("/bin/cat", "cat", argv[1], NULL);
		perror("execl error");
		exit(0);
	}
	wait(NULL);
	printf("Parent message");
	return 0;
}
