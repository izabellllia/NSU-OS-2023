#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fd = open("test", O_RDWR);

    if (fd == -1) {
        perror("open error");
        return -1;
    }

    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl error");
        return -2;
    }

    system("vim test");

    lock.l_type = F_UNLCK;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl back error");
        return -3;
    }


    if (close(fd) == -1) {
        perror("close error");
        return -4;
    }

    return 0;
}
