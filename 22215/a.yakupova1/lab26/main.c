#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


int main() {
	char str[] = "aaBCA csA hi! oaoaA\n";

	FILE *f = popen("tr [:lower:] [:upper:]", "w");
	if (f == NULL){
		perror("error in popen");
		return 1;
	}

	if (fputs(str, f) == EOF) {
		perror("error in writing");
		pclose(f);
		return 1;
	}
	pclose(f);

	return 0;
}
