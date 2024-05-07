#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FILE* fin = fopen(argv[1], "r");
    if (!fin) {
        perror("File is not opened");
        return 1;
    }

    FILE* pipe = popen("wc -l", "w");
    if (!pipe) {
        perror("Popen error");
        return 1;
    }

    bool is_new_str = true;
    char str[BUFSIZ];
    while (fgets(str, BUFSIZ, fin)) {
        if (str[0] == '\n' && is_new_str) {
            fputs("\n", pipe);
        }
        is_new_str = str[strlen(str)-1] == '\n' ? true : false;
    }

    fclose(fin);
    pclose(pipe);
    return 0;
}
