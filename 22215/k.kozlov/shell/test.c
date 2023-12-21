#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include <string.h>

char* STATUS_MESSAGES[100] = {
	"ls", 
	"-la",
	"*.c",
    NULL
};

void constructWordExp(char** args, wordexp_t *p) {
    char line[1024];
    strcat(line, args[0]);
    for (int argIndex = 1; args[argIndex]; argIndex++) {
        strcat(line, " ");
        strcat(line, args[argIndex]);
    }
    wordexp(line, p, 0);
}

int
main(int argc, char **argv)
{
    wordexp_t p;
    char **w;
    int i;

    constructWordExp(STATUS_MESSAGES, &p);

    w = p.we_wordv;
    for (i=0; i < p.we_wordc; i++)
        printf("%s\n", w[i]);
    wordfree(&p);
    exit(EXIT_SUCCESS);
}
