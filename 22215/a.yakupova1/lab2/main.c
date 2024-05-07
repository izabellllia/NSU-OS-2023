
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(void)
{
	putenv("TZ=America/Los_Angeles");
	time_t system_time;
	struct tm* answer_time;
	system_time = time(NULL);
	answer_time = localtime(&system_time);
	printf(asctime(answer_time));
}
