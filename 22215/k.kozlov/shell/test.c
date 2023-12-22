#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include <string.h>

char* STATUS_MESSAGES[100] = {
	"ls", 
	"hello world",
	"*.c",
    NULL
};

int isAnySpace(char* str) {
	for (int i = 0; str[i]; ++i) {
		if (str[i] == ' ')
			return 1;
	}
	return 0;
}

void constructWordExp(char** args, wordexp_t *p) {
	char line[1024], isSpace = 0;
	strcat(line, args[0]);
	for (int argIndex = 1; args[argIndex]; argIndex++) {
		isSpace = isAnySpace(args[argIndex]);
		strcat(line, " ");
		if (isSpace)
			strcat(line, "\"");
		strcat(line, args[argIndex]);
		if (isSpace)
			strcat(line, "\"");
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
