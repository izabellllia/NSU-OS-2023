#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
extern char *tzname[];

int main() {
    time_t now;
    struct tm *sp;

    if (time( &now ) == -1) {
        perror("Failed to take current time.");
        return -1;
    }
 
    if (putenv("TZ=PST8PDT") != 0) {
        perror("Failed to set environment variable.");
        return -1;
    }

    sp = localtime(&now);
    printf("California time: %d/%d/%02d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday,
        sp->tm_year + 1900, sp->tm_hour,
        sp->tm_min, tzname[sp->tm_isdst]);

    printf("California time universal standart: %s", ctime(&now));
    return 0;
}
