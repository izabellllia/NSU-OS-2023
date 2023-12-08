#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 512
int main() {
    char line[BUFSIZE];

    FILE *fpin = popen("echo AbCdEfFfFtF!", "r");
    if (fpin == NULL) {
            perror("popen fail");
            exit(1);
    }
    FILE *fpout = popen("tr [:lower:] [:upper:]", "w");
    if (fpout == NULL) {
            perror("popen fail");
            pclose(fpin);
            exit(1);
    }

    while (fgets(line, BUFSIZE, fpin) != NULL) {
       if (fputs(line, fpout) == EOF) {
               perror("fputs fail");
               pclose(fpin);
               pclose(fpout);
               exit(1);
        }
    }

    pclose(fpin);
    pclose(fpout);

    return 0;
}
