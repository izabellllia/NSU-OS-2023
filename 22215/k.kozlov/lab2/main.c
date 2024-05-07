#include "stdlib.h"
#include "stdio.h"
#include "time.h"

int main(int argc, char* argv[], char** env) {
	putenv("TZ=PST8");
	time_t utc;
	time(&utc);
	printf(asctime(localtime(&utc)));
	return 0;
}