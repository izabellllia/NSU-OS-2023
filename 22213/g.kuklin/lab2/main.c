#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>

#define BUFLEN 26

int main() {
    if (putenv("TZ=America/Ensenada") != 0) {
        perror("Failed to set env variable");
        return 1;
    }
    time_t t = time(0);
    char datetime[BUFLEN] = {};
    if (!ctime_r(&t, datetime, BUFLEN)) {
        perror("Failed to print current time");
    }
    printf(datetime);

    return 0;
}
