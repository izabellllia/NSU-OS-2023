#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>


int main(int argc, char **argv) {
    int fd[2];
    pid_t pid;

    if (pipe(fd) == -1) {
        perror("Pipe failed");
        exit(-1);
    }

    if ((pid = fork()) == 0) {
        close(fd[0]);
        char stringOut[BUFSIZ] = "String\n";
        write(fd[1], stringOut, BUFSIZ);
        close(fd[1]);
        exit(0);

    } else if (pid > 0) {
        close(fd[1]);
        char stringIn[BUFSIZ];
        size_t byteRead;

        while ((byteRead = read(fd[0], stringIn, BUFSIZ)) > 0) {
            for (size_t i = 0; i < byteRead; i++) {
                putc(toupper(stringIn[i]), stdout);
            }
        }
        close(fd[0]);
        exit(0);

    } else {
        perror("Fork failed");
        exit(-1);
    }
}
