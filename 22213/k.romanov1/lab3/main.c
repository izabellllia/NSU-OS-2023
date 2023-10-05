#include <stdio.h>
#include <errno.h>
#include <ucred.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>


void tryOpenFile(const char* name) {
    FILE * file = fopen(name, O_RDONLY);
    if (!file) {
        perror("can't open file");
        return;
    }
    if (fclose(file) != 0) {
        perror("cant't close file");
        return;
    }
}

int main (int argc, char *argv[]) {
    if (argc == 1) {
        perror("there is no arguments\n");
        exit(1);
    }
    printf("actual user's id = %d, effective user's id = %d\n", getuid(), geteuid());

    tryOpenFile(argv[1]);
    
    if (setuid(getuid()) == -1) {
        perror("");
        exit(1);
    }

    printf("actual user's id = %d, effective user's id = %d\n", getuid(), geteuid());
    
    tryOpenFile(argv[1]);

    exit(0);
}

