#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>

void printUserID() {
	printf("real uid: %d\neffective uid: %d\n", getuid(), geteuid());
}

void checkPermission() {
	FILE* file = fopen("top_secret", "r");
	if (file == NULL) {
		perror("Can't open file");
	} else {
		printf("Success\n");
		fclose(file);
	}
}

int main() {
	
	printUserID();
	checkPermission();
	
	setuid(getuid());

	printUserID();
	checkPermission();

	return 0;
}
