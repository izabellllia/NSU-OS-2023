#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    time_t now;
    struct tm *sp;

    (void)time(&now);

    printf("%s", ctime(&now));
    if(putenv("TZ=America/Los_Angeles") == -1){
        perror("error: putenv");
        exit(0);
    }

    sp = localtime(&now);
    if (sp == NULL) {
		perror("error: localtime");
		exit(0);
	}


    printf("%d/%d/%02d %d:%02d %s\n",
           sp->tm_mon + 1, sp->tm_mday,
           sp->tm_year + 1900, sp->tm_hour,
           sp->tm_min, tzname[sp->tm_isdst]);

    exit(0);
}