#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Real UID: %d\n", getuid());
    printf("Effective UID: %d\n", geteuid());

    FILE *file = fopen("data.txt", "r");
    if (file == NULL) {
        perror("Error opening file or you don't have a permission.");
    } else {
        printf("File opened successfully.\n");
        fclose(file);
    }

    if (setuid(getuid()) == -1) {
        perror("Error setting user ID");
        return -1;
    }

    printf("Real UID after setuid: %d\n", getuid());
    printf("Effective UID after setuid: %d\n", geteuid());

    file = fopen("data.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
    } else {
        printf("File opened successfully\n");
        fclose(file);
    }

    return 0;
}
