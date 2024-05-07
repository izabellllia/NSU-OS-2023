#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/wait.h>

int main() {
    int pipe_fd[2];

    // creating programm channel
    if(pipe(pipe_fd) == -1) {
        perror("error with pipe");
        return 1;
    }

    pid_t desc = fork();

    switch(desc) {
        case -1:
            perror("fork error");
            return 2;
        case 0:
            close(pipe_fd[0]);

            char text[] = "hEEllo WOrLD!10!,";
            write(pipe_fd[1], text, sizeof(text));
            close(pipe_fd[1]);
            break;
        default:
            close(pipe_fd[1]);

            char symbol;
            ssize_t status;
            while((status = read(pipe_fd[0], &symbol, 1)) != 0) {
                printf("%c", toupper(symbol));
            }
            printf("\n");

            close(pipe_fd[0]);
            break;
    }

    return 0;
}
