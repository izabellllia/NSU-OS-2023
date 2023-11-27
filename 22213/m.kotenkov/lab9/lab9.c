#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>
#include <wait.h>


int main() {
    pid_t pid;
    int child_status;

    switch (pid = fork()) {
        case (-1):
            perror("Fork failure");
            exit(EXIT_FAILURE);

        case (0):
            execlp("cat", "cat", "big_text.txt", NULL);
            perror("Troubles with execlp");
            exit(EXIT_FAILURE);

        default:
            if (waitpid(pid, &child_status, WUNTRACED) == -1) {
                perror("Waitpid failure");
                exit(EXIT_FAILURE);
            }

            printf("\nRRRAAAAAAAAWR IMA PARRENT\n");

            exit(EXIT_SUCCESS);
    }
}
