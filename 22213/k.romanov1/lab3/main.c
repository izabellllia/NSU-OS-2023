#include <stdio.h>
#include <errno.h>
#include <ucred.h>
#include <unistd.h>
#include <stdlib.h>


void tryOpenFile(const char* name) {
    FILE * file = fopen(name, "r");

    if (file == NULL) {
        perror("can't open file");
        return;
    }
    if (fclose(file)) {
        perror("cant't close file");
    }
}

int main (int argc, char *argv[]) {
    if (argc == 1) {
        perror("there is no arguments\n");
        exit(1);
    }
    printf("actual user's id = %d, effective user's id = %d\n", getuid(), geteuid());

    tryOpenFile(argv[1]);
    
    if (setuid(getuid())) {
        perror("failed to set euid");
        exit(1);
    }

    printf("actual user's id = %d, effective user's id = %d\n", getuid(), geteuid());
    
    tryOpenFile(argv[1]);

    exit(0);
}
