#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<stdlib.h>

int main() {
	
	FILE *wc = popen("grep '^$' text | wc -l", "r");
	if (wc == NULL) {
		perror("Error while creating pipe");
		exit(1);
	}

	int x;
	if (fscanf(wc, "%d", &x) != 1) {
		perror("Error while taking answer from wc");
		exit(2);
	}
	
	printf("%d\n", x);

	if (pclose(wc) == -1) {
		perror("Error while closing pipe");
		exit(3);
	}

	return 0;
}
