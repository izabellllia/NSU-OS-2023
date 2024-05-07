#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio_ext.h>

#define INIT_SIZE 100

typedef struct {
    off_t *arr;
    size_t size;
    size_t capacity;
} Vector;

void initVector(Vector *vector) {
    vector->arr = (off_t *) malloc(INIT_SIZE * sizeof(off_t));
    vector->size = 0;
    vector->capacity = INIT_SIZE;
}

void add(Vector *vector, off_t element) {
    if (vector->size == vector->capacity) {
        vector->capacity *= 2;
        vector->arr = (off_t *) realloc(vector->arr, vector->capacity * sizeof(off_t));
    }

    vector->arr[vector->size++] = element;
}

void freeVector(Vector *vector) {
    free(vector->arr);
    vector->size = 0;
    vector->capacity = 0;
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file name>\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Failed to open file");
        return -1;
    }

    char buffer[BUFSIZ];
    size_t cntReadBytes;

    Vector vector;
    initVector(&vector);

    off_t currentOffset = 0;
    add(&vector, 0);

    while ((cntReadBytes = read(fd, buffer, BUFSIZ)) > 0) {
        for (size_t i = 0; i < cntReadBytes; i++) {
            if (buffer[i] == '\n') {
                add(&vector, currentOffset + (off_t) i);
            }
        }
        currentOffset += (off_t) cntReadBytes;
    }

    for (int i = 1; i < vector.size; i++) {
        vector.arr[i] += 1;
    }

    if (vector.arr[vector.size - 1] != currentOffset) {
        add(&vector, currentOffset);
    }

    while (1) {
        size_t findLine;
        if (scanf("%zu", &findLine) != 1) {
            fprintf(stderr, "Incorrect number of line. Try again.\n");
            __fpurge(stdin);
            continue;
        }

        if (findLine == 0) {
            break;
        }

        if ((findLine < 1) || (findLine > vector.size - 1)) {
            fprintf(stderr, "Line in range from 1 to %zu. Try again.\n", vector.size - 1);
            continue;
        }

        off_t currentOffsetLine = vector.arr[findLine - 1];
        off_t bytesToWrite = vector.arr[findLine] - currentOffsetLine - 1;
        off_t bytesRead;


        lseek(fd, currentOffsetLine, SEEK_SET);

        while (bytesToWrite > 0) {
            off_t bytesToRead;
            if (bytesToWrite > BUFSIZ) {
                bytesToRead = BUFSIZ;
            } else {
                bytesToRead = bytesToWrite;
            }

            bytesRead = read(fd, buffer, bytesToRead);

            if (fwrite(buffer, 1, bytesRead, stdout) != bytesRead) {
                perror("Failed to write to stdout");
                return -1;
            }

            bytesToWrite -= bytesRead;
        }

        if (fwrite("\n", 1, 1, stdout) != 1) {
            perror("Failed to write ot stdout");
            return -1;
        }
    }

    freeVector(&vector);
    close(fd);
    return 0;
}
