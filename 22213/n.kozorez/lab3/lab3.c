#include <errno.h>
#include <ucred.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void open_file(const char *filepath) {
    int file = open(filepath, O_RDONLY);
    if (file < 0) {
        perror("Couldn't open the file\n");
        return;
    }
    if (close(file)) {
        perror("Failed to close file");    
    }
}

int main() {
    const char *filename = "file";
    printf("User real id is %d\n", getuid());
    printf("User effective is %d\n", geteuid());

    open_file(filename);

    if (setuid(getuid()) == -1) {
        perror("Couldn't change setuid\n");
        exit(1);
    }

    printf("User real id is %d\n", getuid());
    printf("User effective is %d\n", geteuid());

    open_file(filename);

    exit(0);
}
