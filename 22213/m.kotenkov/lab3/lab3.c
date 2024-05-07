#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    FILE *f;
    uid_t uid = getuid();
    printf("Real user id: %u\n", uid);
    printf("Effective user id: %u\n", geteuid());

    if ((f = fopen("cringe_file.txt", "w")) == NULL) {
        perror("Fopen cannot open the file");
    } else {
        write(1, "Fopen succeed\n", 15);
        fclose(f);
    }

    if (setuid(uid) == -1) {
        perror("Setuid failure");
        exit(EXIT_FAILURE);
    }

    printf("Real user id: %u\n", getuid());
    printf("Effective user id: %u\n", geteuid());

    if ((f = fopen("cringe_file.txt", "w")) == NULL)
    {
        perror("Fopen cannot open the file");
    } else {
        write(1, "Fopen succeed\n", 15);
        fclose(f);
    }

    exit(EXIT_SUCCESS);
}
