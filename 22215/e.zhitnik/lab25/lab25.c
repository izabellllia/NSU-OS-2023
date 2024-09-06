#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFF_SIZE 32

int main() {
    int pipe_fd[2];
    pid_t pid;

    if (pipe(pipe_fd) == -1) {
        perror("Pipe failed");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe_fd[0]);

        char buffer[BUFF_SIZE];
        ssize_t bytesRead;
        while ((bytesRead = read(STDIN_FILENO, buffer, BUFF_SIZE)) > 0) {
            if (write(pipe_fd[1], buffer, bytesRead) == -1) {
                perror("Write to pipe failed");
                exit(EXIT_FAILURE);
            }
        }

        if (bytesRead == -1) {
            perror("Read from stdin failed");
            exit(EXIT_FAILURE);
        }

        close(pipe_fd[1]);
        exit(EXIT_SUCCESS);

    } else {
        close(pipe_fd[1]);

        char buffer[BUFF_SIZE];
        ssize_t bytesRead;
        while ((bytesRead = read(pipe_fd[0], buffer, BUFF_SIZE)) > 0) {
            for (ssize_t i = 0; i < bytesRead; i++) {
                buffer[i] = toupper(buffer[i]);
            }
            if (write(STDOUT_FILENO, buffer, bytesRead) == -1) {
                perror("Write to stdout failed");
                exit(EXIT_FAILURE);
            }
        }

        if (bytesRead == -1) {
            perror("Read from pipe failed");
            exit(EXIT_FAILURE);
        }

        close(pipe_fd[0]);
        exit(EXIT_SUCCESS);
    }

    return 0;
}

