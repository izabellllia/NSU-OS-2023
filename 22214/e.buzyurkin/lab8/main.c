#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("No filename was provided\n");
        exit(1);
    }

    char* filename = argv[1];

    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        perror("failed to open the file");
        exit(1);
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("failed to lock the file");
        exit(1);
    }

    
    char command[100];

    sprintf(command, "nano %s", argv[1]);
    system(command);


    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return 0;
}