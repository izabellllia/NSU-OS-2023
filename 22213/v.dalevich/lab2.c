#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>


int main()
{

    if (setenv("TZ", "America/Los_Angeles", 1) != 0)
{
        perror("Failed to set new environ variable");
        return 1;
}


    time_t now = time(NULL);
    char buflen[26];

    if (ctime_r(&now, buflen, 26) == 0)
{
        perror("Failed to convert time");
        return 1;
}

    printf("%s",buflen);


    return 0;
 }
