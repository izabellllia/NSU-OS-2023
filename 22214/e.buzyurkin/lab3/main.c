#include <stdio.h>
#include <unistd.h>


void try_to_open(char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Failed to open the file\n");
        return;
    }

    fclose(file);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("No filename was provided\n");
        return 1;
    }

    printf("uid: %d, euid: %d\n", getuid(), geteuid());
    try_to_open(argv[1]);

    setuid(getuid());

    printf("uid: %d, euid: %d\n", getuid(), geteuid());
    try_to_open(argv[1]);

    return 0;
}