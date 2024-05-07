#include <stdio.h>
#include <unistd.h>

void try_to_open_file(char* filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Could not open the file");
        return;
    }
    if (fclose(file) != 0) {
        perror("Could not close the file");
        return;
    }
}

int main (int argc, char *argv[]) {

    printf("uid = %d, euid = %d\n", getuid(), geteuid());

    if (argc < 2) {
        printf("No file name\n");
        return 1;
    }

    try_to_open_file(argv[1]);

    if (setuid(getuid()) != 0) {
        perror("Error with changing euid\n");
        return 1;
    }

    printf("uid = %d, euid = %d\n", getuid(), geteuid());

    try_to_open_file(argv[1]);

    return 0;
}
