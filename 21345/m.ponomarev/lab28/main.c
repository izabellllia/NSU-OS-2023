#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <time.h>

int main() {
    FILE* fd[2];
    if (p2open("sort -n", fd) == -1) {
        fprintf(stderr, "p2open error\n");
        return 1;
    }

    time_t currentTime = time(NULL);
    if (currentTime == -1) {
        perror("time error");
        p2close(fd);
        return 1;
    }

    srand(currentTime);

    for (int i = 0; i < 100; ++i) {
        fprintf(fd[0], "%d\n", rand() % 100);
    }

    fclose(fd[0]);

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            int val;
            fscanf(fd[1], "%d", &val);
            printf("%d ", val);
        }
        printf("\n");
    }

    pclose(fd[1]);

    return 0;
}
