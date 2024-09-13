#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char input[BUFSIZ];
    FILE *fin, *fpout;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    if ((fin = fopen(argv[1], "r")) == NULL) {
        perror(argv[1]);
        return 1;
    }

    if ((fpout = popen("wc -l", "w")) == NULL) {
        perror("popen");
        fclose(fin);
        return 1;
    }

    while (fgets(input, BUFSIZ, fin) != NULL) {
        fputs(input, fpout);
    }

    fclose(fin);
    pclose(fpout);
    return 0;
}
