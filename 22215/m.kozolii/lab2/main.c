#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>



int main(void)
{
        putenv("TZ=America/Los_Angeles");
        time_t now;
        struct tm *sp;

        now = time(&now);

        sp = localtime( &now );
        printf("%d/%d/%02d %d:%02d\n",
                sp->tm_mon + 1, sp->tm_mday,
                sp->tm_year + 1900, sp->tm_hour,
                sp->tm_min);
        printf("TimeZone: %s\n",getenv("HOME"));

        return(0);
}



