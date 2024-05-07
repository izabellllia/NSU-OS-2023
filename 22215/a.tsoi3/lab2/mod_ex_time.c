#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    time_t now;

    putenv("TZ=PST8PDT");  

    (void) time(&now);

    printf("%s", ctime(&now));

    return 0;
}

