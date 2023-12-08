#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    FILE *file;
    uid_t uid = getuid();
    printf("Real user id: %u\n", uid);
    printf("Effective user id: %u\n", geteuid());

    if ((file = fopen("example.txt", "w")) == NULL) {
        perror("fopen");
    } else {
        printf("File opened succesfully\n\n");
        fclose(file);
    }

    if (setuid(uid) < 0) {
        perror("setuid");
        exit(1);
    }

    printf("Real user id: %u\n", getuid());
    printf("Effective user id: %u\n", geteuid());

    if ((file = fopen("example.txt", "w")) == NULL)
    {
        perror("fopen");
    } else {
        printf("File opened succesfully\n");
        fclose(file);
    }
    exit(0);
}
