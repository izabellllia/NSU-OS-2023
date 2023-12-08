#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(){
    time_t now;
    if(time(&now) == -1){
        perror("time");
        exit(1);
    }    
    char stime[26];
    // This function returns the time since 00:00:00 UTC, January 1, 1970 (Unix timestamp) in seconds. Otherwise - -1.
    if((putenv("TZ=America/Los_Angeles")) != 0){
        perror("putenv");
        exit(1);
    }
    if(ctime_r(&now, stime, 26) == NULL){
        perror("ctime_r");
        exit(1);
    }

    printf("%s\n", stime);
    exit(0);
}
