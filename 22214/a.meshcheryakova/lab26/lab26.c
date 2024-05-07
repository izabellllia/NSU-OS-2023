#include <stdio.h>
#include <stdlib.h>

int main() {
    char line[BUFSIZ];

    FILE *fpin = popen("echo Hello, World!", "r");
    if (fpin == NULL) {
        perror("Error with popen");
        exit(1);
    }

    FILE *fpout = popen("tr [:lower:] [:upper:]", "w");
    if (fpout == NULL) {
        pclose(fpin);
        perror("Error with popen");
        exit(1);
    }

    while (fgets(line, BUFSIZ, fpin) != NULL) {
        if (fputs(line, fpout) == -1) {
            pclose(fpin);
            pclose(fpout);
            perror("Error with fputs");
            exit(1);
        }
    }

    int ret1 = pclose(fpin);
    int ret2 = pclose(fpout);
    if ((ret1 == -1) || (ret2 == -1)) {
        perror("Error with pclose");
        exit(1);
    }
    return 0;
}
