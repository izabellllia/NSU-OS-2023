#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 256

int main(int argc, char **argv) {
    FILE *fin, *fout;
    char line[BUFFER_SIZE];

    if ((fin = fopen(argv[1], "r")) == NULL) {
        perror("Fopen failed");
    }

    fout = popen("wc -l", "w");

    if (fout == NULL) {
        fclose(fin);
        perror("Error with popen");
    }

    while (fgets(line, BUFFER_SIZE, fin) != NULL) {
        if (line[0] == '\n') {
            fputs(line, fout);
        }
    }

    fclose(fin);
    pclose(fout);
    exit(0);
}
