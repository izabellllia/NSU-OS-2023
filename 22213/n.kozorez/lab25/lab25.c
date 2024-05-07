#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>


int main(int argc, char **argv) {

    int pipefd[2];
    pid_t cpid;

    if (pipe(pipefd) == -1) {
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    switch (cpid) {
    case -1:
        perror("fork error");
        exit(EXIT_FAILURE);
        
    case 0:
        close(pipefd[0]);
        char stringOut[BUFSIZ] = "String\n";
        write(pipefd[1], stringOut, BUFSIZ);
        close(pipefd[1]);
        exit(EXIT_SUCCESS);

    default:
        close(pipefd[1]);
        char stringIn[BUFSIZ];
        size_t byteRead;

        while ((byteRead = read(pipefd[0], stringIn, BUFSIZ)) > 0) {
            for (size_t i = 0; i < byteRead; i++) {
                putc(toupper(stringIn[i]), stdout);
            }
        }
        close(pipefd[0]);
        exit(EXIT_SUCCESS);
    }
}
