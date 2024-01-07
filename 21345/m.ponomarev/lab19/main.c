#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <fnmatch.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

int main(int argc, char** argv) {
	if (argc != 2) {
		fprintf(stderr, "Argc error\n");
		return 1;
	}

	if (strchr(argv[1], '/') != NULL) {
		fprintf(stderr, "Pattern cannot contain /\n");
		return 1;
	}

	DIR* dir;

	if (!(dir = opendir("."))) {
		perror("Opendir failed");
		return 1;
	}

	struct dirent* entry;
	int counter = 0;
	errno = 0;

	while ((entry = readdir(dir)) != NULL) {
		if (fnmatch(argv[1], entry->d_name, FNM_PATHNAME) == 0) {
			printf("%s\n", entry->d_name);
			++counter;
		}
	}

	if (errno != 0) {
		perror("Readdir failed");
		closedir(dir);
		return 1;
	}

	if (counter == 0) {
		printf("No files match pattern %s\n", argv[1]);
	}
	closedir(dir);
	return 0;
}
