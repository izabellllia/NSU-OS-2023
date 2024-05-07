#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
extern char *tzname[];

int main()
{
     putenv("TZ=America/Los_Angeles");
     time_t now;
     struct tm *sp;

     (void) time( &now );

     printf("%s", ctime( &now ) );
     return 0;
 }