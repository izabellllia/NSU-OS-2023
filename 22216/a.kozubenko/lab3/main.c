#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void checkout(FILE *file) {
    file = fopen("file", "r");
    if (file == NULL) {
        perror("Failed to open file.");
        return;
    }
    if (fclose(file)) {
        perror("Failed to close file.");    
    }
}

int main()
{
    uid_t real = getuid();
    uid_t effective = geteuid();

    printf("real uid: %d\neffective uid: %d\n.", real, effective);

    FILE *file;
    checkout(file);

    if (setuid(real) == -1) {
        perror("Failed to set user id.");
        exit(1);
    }

    uid_t effective_new = geteuid();

    printf("real uid: %d\neffective uid: %d\n.", real, effective_new);
    checkout(file);

    exit(0);
}

