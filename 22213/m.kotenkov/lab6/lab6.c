#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>


typedef struct str_s {
    off_t offset;
    int len;
} str;


int fd, str_cnt = 0;
str *matrix;


void str_print(str *matrix, int fd, int choice);
void more_memory(int *allocated, int str_cnt, str *tmp, str *matrix);
void sig_handler();


int main() {

    signal(SIGALRM, sig_handler);
    
    fd = open("./big_text.txt", O_RDONLY);
    if (fd == -1) {
        perror("File is not opened.");
        exit(1);
    }
    
    int str_len = 0,
        pos = 0,
        allocated = 1,
        last = 0;
	long choice = -1;
    char buffer[BUFSIZ];

    str *tmp = NULL;
    if ((matrix = (str*) malloc(sizeof(str))) == NULL) {
        perror("Malloc error");
        exit(2);
    }
    
    ssize_t bytes_red;
    bytes_red = read(fd, &buffer, BUFSIZ);
    
    while (bytes_red > 0) {
        while (buffer[pos] != '\n' && bytes_red > 0) {
            bytes_red--;
            str_len++;
            pos++;
        }
        
        if (buffer[pos] == '\n') {
            more_memory(&allocated, str_cnt, tmp, matrix);
            matrix[str_cnt].offset = last + pos - str_len;
            matrix[str_cnt].len = str_len;

            str_len = 0;
            str_cnt++;
            pos++;
            bytes_red--;
        }
        
        if (bytes_red == 0) {
            last += pos;
            pos = 0;
            bytes_red = read(fd, &buffer, BUFSIZ);
        }
    }
    
    char choice_buffer[20];

    do {
        alarm(5);
        printf("Choose the line: ");
        scanf("%20s", choice_buffer);
        while (((choice = atol(choice_buffer)) == 0 && choice_buffer[0] != '0') || choice < 0) {
            printf("Wrong inp\nChoose the line: ");
            scanf("%20s", choice_buffer);
        }
        alarm(0);
        
        if (choice_buffer[0] == '0') {
            break;
        }
        
        if (choice > str_cnt) {
            printf("No such line. There are %d lines.\n", str_cnt);
        } else {
            str_print(matrix, fd, choice);
        }
    } while (choice != 0);
    
    free(matrix);
    close(fd);
    exit(0);

}


void sig_handler(){
    if ((fd != -1) && (matrix != NULL))
    {
        int l = matrix[str_cnt-1].offset + matrix[str_cnt-1].len;
        char take_str[l + 1];
        off_t status = lseek(fd, 0, SEEK_SET);
        if (status == -1)
        {
            write(0, "lseek failed\n", 13);
            exit(1);
        }
        
        if (read(fd, take_str, l) == -1) {
            write(0, "Cannot read line\n", 17);
            exit(1);
        };
        take_str[l] = '\0';
        write(0, take_str, l);
        close(fd);
        exit(0);
    }
    
    exit(0);
}


void more_memory(int *allocated, int str_cnt, str *tmp, str *matrix) {
    if ((*allocated) < str_cnt + 1) {
        (*allocated) *= 2;
        if ((tmp = realloc(matrix, (*allocated) * sizeof(str))) != NULL) {
            matrix = tmp;
        } else {
            perror("Realloc error");
        }
    }
}


void str_print(str *matrix, int fd, int choice) {
    char take_str[(int)matrix[choice - 1].len];
    if (lseek(fd, matrix[choice - 1].offset, SEEK_SET) != matrix[choice - 1].offset) {
        perror("Not correct number of bytes red.");
    };
    if (read(fd, take_str, matrix[choice - 1].len) == -1) {
        perror("Cannot read line.");
    };
    printf("%.*s\n", (int)matrix[choice - 1].len, take_str);
}

