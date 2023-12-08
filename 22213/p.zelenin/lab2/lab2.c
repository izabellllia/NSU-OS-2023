#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#define MIN_BUFLEN 26

int main() {

    char buf[MIN_BUFLEN];
    time_t t;
    if ((t = time(0)) == -1) {
        perror("error: could not get the time - ");
        exit(EXIT_FAILURE);
    }

    if (putenv("TZ=US/Pacific")) {
        perror("putenv() func caused an error - ");
        exit(EXIT_FAILURE);
    }    

    char* result = ctime_r(&t, buf, MIN_BUFLEN);
    if (result == NULL) {
        perror("ctime_r() func caused an error - ");
        exit(EXIT_FAILURE);
    }

    printf("%s", result);
    exit(EXIT_SUCCESS);
}
