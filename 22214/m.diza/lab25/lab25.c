#define SIZE 14
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe error");
        exit(1);
    }
   
    char c;

    switch (fork()) {
        case -1:
            perror("fork() error");
            exit(2);
        case 0: 
            close(fd[1]);
            for (int i = 0; i < SIZE; i++) {
                read(fd[0], &c, 1);
                printf("%c", toupper(c));
            }
            close(fd[0]);
            break;
        default:
            close(fd[0]);
            char stringout[] = "Hello, world!\n";
            write(fd[1], stringout, SIZE);
            close(fd[1]);
            wait(NULL);
            break;
    }

    return 0;
}
