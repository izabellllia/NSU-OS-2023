#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <wait.h>

int main() {
    int fd[2];
    pid_t pid;
    ssize_t ret;

    if (pipe(fd) == -1) {
        perror("Error with pipe");
        exit(1);
    }

    pid = fork();

    if (pid == -1) {
        perror("Error with fork");
        exit(1);
    }

    if (pid == 0) {
        char str[20] = "Hello, World!\n";
        close(fd[1]);
        ret = write(fd[0], str, 20);
        if (ret == -1) {
            perror("Error with child's write");
            exit(1);
        }
        close(fd[0]);
    }

    else {
        close(fd[0]);
        char c;
        ret = read(fd[1], &c, 1);
        if (ret == -1) {
            perror("Error with parent's read");
            exit(1);
        }
        while(ret != 0) {
            printf("%c", toupper(c));
            ret = read(fd[1], &c, 1);
            if (ret == -1) {
                perror("Error with parent's read");
                exit(1);
            }
        }
        close(fd[1]);
        if (wait(NULL) == -1) {
            perror("There is not a single incomplete subprocess");
            exit(1);
        }
    }
    return 0;
}
