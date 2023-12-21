#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>

int main() {

	FILE *file = popen("tr [:lower:]  [:upper:]", "w");
	if (file == NULL) {
		perror("Can't call tr comand");
		exit(1);
	}

	if (fputs("Hello, World!\n", file) == EOF) {
		perror("Can't write to pipe");
		exit(1);
	}

	if (pclose(file) == -1) {
		perror("Error while terminating pipe");
		exit(1);
	}
	
	return 0;
}
