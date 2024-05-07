#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/wait.h>

#define L 11

int main() {
    pid_t child;
    int fd[2];
    if (pipe(fd) == -1) {
        perror("failed to create pipe\n");
        return -1;
    }
    child = fork();
    if (child == 0) {
        char c;
        close(fd[1]);
        while (read(fd[0], &c, 1) != 0) {
            printf("%c", toupper(c));
        }
        close(fd[0]);
    } else {
        close(fd[0]);
        char *text = "Some text\n";
        write(fd[1], text, L);
        close(fd[1]);
        wait(NULL);
    }
    return 0;
}
