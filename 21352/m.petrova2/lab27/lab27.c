#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc < 2) {
            fprintf(stderr, "No text file\n");
            return 0;
        }
        FILE* fin = fopen(argv[1], "r");
        if (!fin) {
            perror("File is not opened");
            return 0;
        }

        FILE* fpout = popen("wc -l", "w");
        if (!fpout) {
            perror("Popen error");
            return 0;
        }
        char str[BUFSIZ];
        char last = '\n';
        while (fgets(str, BUFSIZ, fin)) {
            if (str[0] == '\n' && last == '\n') {
                fputs("\n", fpout);
            }
            last = str[strlen(str)-1];
        }
        fclose(fin);
        pclose(fpout);
        return 0;
}

