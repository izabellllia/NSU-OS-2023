#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main() {
    uid_t r_uid = getuid();
    uid_t e_uid = geteuid();

    printf("UID: %d\nEUID: %d\n", r_uid, e_uid);

    FILE *file = fopen("myfile", "r");
    if (file == NULL) {
        perror("Cant open file 'myfile'\n");
    } else {
        printf("Opened\n");
        fclose(file);
    }

    setuid(r_uid);
    e_uid = getuid();

    printf("UID: %d\nEUID: %d\n", r_uid, e_uid);

    file = fopen("myfile", "r");
    if (file == NULL) {
        perror("Cant open 'myfile'\n");
    } else {
        printf("Opened\n");
        fclose(file);
    }

    return 0;
}
