#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

int main() {
    srand((unsigned int)time(NULL));
    FILE *pipe_fd[2];

    if (p2open("sort -n", pipe_fd) == -1) {
        perror("Failed to open pipe");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < 100; i++) {
        int number = rand() % 100;
        fprintf(pipe_fd[0], "%d\n", number);
    }
    fclose(pipe_fd[0]);

    
    printf("Sorted numbers:\n");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            int number;
            if (fscanf(pipe_fd[1], "%d", &number) == 1) {
                printf("%3d ", number);
            } else {
                fprintf(stderr, "Error reading number\n");
                pclose(pipe_fd[1]);
                return EXIT_FAILURE;
            }
        }
        printf("\n");
    }

    if (pclose(pipe_fd[1]) == -1) {
        perror("Error closing pipe");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
