#include <stdio.h>
#include <errno.h>
#include <ucred.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void check_file(const char *filepath) {
    int file = open(filepath, O_RDONLY);

    if (file < 0) {
        perror("Failed to open file");
        return;
    }
    if (close(file)) {
        perror("Failed to close file");    
    }
}

int main() {
    const char *filename = "test_file.txt";
    printf("Effective uid: %d. Real uid: %d.\n", geteuid(), getuid());

    check_file(filename);

    uid_t my_uid = getuid();
    if (setuid(my_uid)) {
        perror("Failed to set euid");    
    };
    printf("Effective uid: %d. Real uid: %d.\n", geteuid(), getuid());

    check_file(filename);

    exit(0);
}
