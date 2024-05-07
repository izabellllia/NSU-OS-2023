#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        perror("No file to open");
        return -1;
    }

    printf("User id: %d\n", getuid());
    printf("User effective id: %d\n", geteuid());

    FILE* file = fopen(argv[1], "r+");
    if (file != NULL) {
        if (fclose(file) != 0) {
            perror("The file wasn't closed");
        }
    } else {
        perror("File opening is impossible");
    }


    if (setuid(getuid()) != 0) {
        perror("Error trying to set effective id");
    }


    printf("User id: %d\n", getuid());
    printf("User effective id: %d\n", geteuid());

    file = fopen(argv[1], "r+");
    if (file != NULL) {
        if (fclose(file) != 0) {
            perror("The file wasn't closed");
        }
    } else {
        perror("File opening is impossible");
    }

    return 0;
}

