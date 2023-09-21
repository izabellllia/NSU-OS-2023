#include <sys/types.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
 
int main()
{
        time_t now = time(NULL);
        const int lenBuf = 26;


        char * buf = (char*)malloc(sizeof(char)*lenBuf);
        putenv("TZ=GMT+7");
        printf("%s\n", ctime_r(&now, buf, lenBuf));
        return 0;
}


