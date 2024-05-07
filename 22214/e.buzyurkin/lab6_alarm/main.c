#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

typedef struct Line_t{
    off_t offset;
    off_t length;
} Line;

typedef struct Vector_t{
    Line *array;
    int cnt;
    int cap;
} Vector;

void vector_init(Vector *a) {
    a->array = malloc(sizeof(Line));
    a->cnt = 0;
    a->cap = 1;
}

void vector_push(Vector *a, Line element) {
    if (a->cnt == a->cap) {
        a->cap *= 2;
        a->array = realloc(a->array, a->cap * sizeof(Line));

        if (a->array == NULL) {
            perror("realloc has failed");
            exit(1);
        }
    }

    a->array[a->cnt++] = element;
}

void vector_destroy(Vector *a) {
    free(a->array);
    a->array = NULL;
    a->cnt = a->cap = 0;
}

int fd;
Vector * table;

void handle_alarm(int signum){
    char buf[BUFSIZ];
    int i;
    lseek(fd, 0LL, SEEK_SET);
    while((i = read(fd, &buf, BUFSIZ)) > 0) {
        write(1, &buf, i);
    }

    close(fd);
    vector_destroy(table);
    exit(0);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("No filename was provided\n");
        return 1;
    }

    table = malloc(sizeof(Vector));
    vector_init(table);

    fd = open(argv[1], O_RDONLY);
    if (fd == -1) { 
        perror("Couldnt open the file");
        exit(1); 
    }

    lseek(fd, 0LL, SEEK_SET);

    char c;
    off_t offset = 0;
    off_t length = 0;
    off_t max_length = 0;
    while (read(fd, &c, 1) == 1) {
        if (c == '\n') {
            Line current = {offset, length};
            vector_push(table, current);

            if (length > max_length)
                max_length = length;

            offset += length + 1;
            length = 0;
        } 
        else {
            length++;
        }
    }

    if (length > 0) {
        Line current = {offset, length};
        vector_push(table, current);
    }

    Line line_info;
    char str[max_length + 2];
    int n;

    signal(SIGALRM, handle_alarm);
    
    while (1) {
        printf("Enter the line number within 5 seconds: \n");
        alarm(5);

        if (scanf("%d", &n) != 1) {
            perror("Incorrect input");
            exit(1);
        }

        alarm(0);
        
        if (n == 0) { 
            break; 
        }

        if (table->cnt < n) {
            printf("The file contains only %d lines\n", table->cnt);
            continue;
        }

        line_info = table->array[n - 1];

        lseek(fd, line_info.offset, SEEK_SET);
        read(fd, &str, line_info.length * sizeof(char));
        str[line_info.length] = 0;

        printf("%s\n", str);
    }


    close(fd);
    vector_destroy(table);

    return 0;
}