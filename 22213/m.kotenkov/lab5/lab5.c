#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


typedef struct str_s {
    int offset;
    int len;
} str;


void more_memory(int *allocated, int str_cnt, str *tmp, str *matrix) {
    if ((*allocated) < str_cnt + 1)
    {
        (*allocated) *= 2;
        if ((tmp = realloc(matrix, (*allocated) * sizeof(str))) != NULL)
        {
            matrix = tmp;
        }
        else
        {
            perror("Realloc error");
        }
    }
}


void str_print(str *matrix, int fd, int choice) {
    char take_str[(int)matrix[choice - 1].len];
    if (lseek(fd, matrix[choice - 1].offset, SEEK_SET) != matrix[choice - 1].offset) {
        perror("Not correct number of bytes red.");
    };
    if (read(fd, take_str, matrix[choice - 1].len) == -1)
    {
        perror("Cannot read line.");
    };
    printf("%.*s\n", (int)matrix[choice - 1].len, take_str);
}


int main() {

    int fd = open("./big_text.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("File is not opened.");
        exit(1);
    }
    
    int str_len = 0,
        pos = 0,
        str_cnt = 0,
        allocated = 1,
        last = 0;
    
    long choice = 0;
    
    char buffer[BUFSIZ];

    str *tmp = NULL;
    str *matrix;
    if ((matrix = (str*) malloc(sizeof(str))) == NULL)
    {
        perror("Malloc error");
        exit(2);
    }
    
    
    ssize_t bytes_red;
    bytes_red = read(fd, &buffer, BUFSIZ);
    

    while (bytes_red > 0)
    {
        while (buffer[pos] != '\n' && bytes_red > 0)
        {
            bytes_red--;
            str_len++;
            pos++;
        }
        
        if (buffer[pos] == '\n')
        {
            more_memory(&allocated, str_cnt, tmp, matrix);
            matrix[str_cnt].offset = last + pos - str_len;
            matrix[str_cnt].len = str_len;

            str_len = 0;
            str_cnt++;
            pos++;
            bytes_red--;
        }
        
        if (bytes_red == 0)
        {
            last += pos;
            pos = 0;
            bytes_red = read(fd, &buffer, BUFSIZ);
        }
    }
    
    char choice_buffer[100];
    
    do
    {
        printf("Choose the line: ");
        scanf("%s", choice_buffer);
        while (((choice = atol(choice_buffer)) == 0 && choice_buffer[0] != '0') || choice < 0)
        {
            printf("Wrong inp\nChoose the line: ");
            scanf("%s", choice_buffer);
        }
        
        if (choice_buffer[0] == '0')
        {
            break;
        }
        
        if (choice > str_cnt)
        {
            printf("No such line. There are %d lines.\n", str_cnt);
        }
        else
        {
            str_print(matrix, fd, choice);
        }
    } while (choice != 0);
    
    free(matrix);
    close(fd);
    exit(0);
}
