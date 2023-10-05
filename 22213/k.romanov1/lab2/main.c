#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
 
int main()
{
        time_t now = time(NULL);
        const int lenBuf = 26;
        char buf[lenBuf];

        if (putenv("TZ=GMT+7") != 0) {
            perror("");
            exit(1);
        }
        printf("%s\n", ctime_r(&now, buf, lenBuf));
        exit(0);
}


