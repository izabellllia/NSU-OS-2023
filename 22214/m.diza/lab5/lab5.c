#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

typedef struct table {
    off_t offset;
    off_t len;
} table;

table* allocate(int n);
void reallocation(table** arr, int n, int* cap);
table* makeTable(int* amount_of_rows, int fd);

int main() {

    int fd;
    if ((fd = open("file.txt", O_RDONLY)) == -1) {
        perror("open() error");
        exit(1);
    } 

    int amount_of_rows = 1;
    
    table* arr = makeTable(&amount_of_rows, fd);

    if(lseek(fd, 0, 0) == -1) {
        perror("lseek() error");
        exit(3);
    }
    int line_number;

    while(1) {
        printf("Enter the line number: ");
        if(scanf("%d", &line_number) != 1) {
            while (getchar() != '\n');
            continue;
        }
        if (line_number == 0) break;
        if (line_number >= amount_of_rows) continue;

        if(lseek(fd, arr[line_number - 1].offset, 0) == -1) {
            perror("lseek() error");
            exit(3);
        }
        char* string = (char*)calloc(arr[line_number - 1].len + 1, 1);
        if (read(fd, string, arr[line_number - 1].len) != arr[line_number - 1].len) {
            perror("read() error");
            exit(2);
        }
        printf("%s\n", string);
        free(string);
    } 
    
    free(arr);
    close(fd);
    return 0;
}


table* allocate(int n) {
    return (table*)malloc(n * sizeof(table));
}

void reallocation(table** arr, int n, int* cap) {
    *arr = (table*)realloc(*arr, n * sizeof(table)); 
    *cap = n / 2;
}

table* makeTable(int* amount_of_rows, int fd) {
    char buffer;
    off_t current_offset = 0;
    
    table* arr = allocate(1); 
    if (arr == NULL) {
        fprintf(stderr, "allocation error");
        exit(9);
    }
    arr[0].offset = 0;
    arr[0].len = 0;

    int capacity = 0;

    while(1) {
        if(read(fd, &buffer, 1) == 0) {
            arr[*amount_of_rows - 1].len = current_offset - arr[*amount_of_rows - 1].offset;
            break;
        }
        if (buffer == '\n') {
            if (!capacity) reallocation(&arr, *amount_of_rows * 2, &capacity);
            if (arr == NULL) {
                fprintf(stderr, "allocation error\n");
                exit(9);
            }
            arr[*amount_of_rows].offset = current_offset + 1;
            arr[*amount_of_rows - 1].len = current_offset - arr[*amount_of_rows - 1].offset;
            ++(*amount_of_rows);
            --capacity;
        }
        ++current_offset;
    }
    
    return arr;
}