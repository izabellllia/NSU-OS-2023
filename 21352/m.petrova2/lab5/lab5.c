#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "No text file\n");
        return 0;
    }
    int file_desc = 0;
    if ((file_desc = open(argv[1], O_RDONLY)) == -1) {
        perror("File is not opened");
        return 0;
    }
    
    size_t lines_capasity = 8;
    off_t* lines_shifts = (off_t*)malloc(lines_capasity * sizeof(off_t));
    off_t* lines_lengths = (off_t*)malloc(lines_capasity * sizeof(off_t));
    if (lines_shifts == NULL || lines_lengths == NULL) {
        perror("Memory allocation failed");
        return 0;
    }
    
    int lines_count = 0;
    char str [255];
    off_t cur_line_length = 0;
    off_t total_shift = 0;
    
    char c_char;
    size_t was_readed = 1;
    
    while (was_readed != 0) {
        was_readed = read(file_desc, str, sizeof(str));
        if (was_readed == -1) {
            perror("Read error");
            return 0;
        }
	off_t i = 0;
        for (i = 0; i < was_readed; i++) {
            total_shift ++;
            c_char = str[i];
            cur_line_length++;
            if (c_char == '\n') {
                lines_lengths[lines_count] = cur_line_length;
                lines_shifts[lines_count] = total_shift - lines_lengths[lines_count];
                cur_line_length = 0;
                lines_count++;
    
                if (lines_count >= lines_capasity) {
                    lines_capasity = lines_capasity * 2;
                    lines_shifts = (off_t*)realloc(lines_shifts,lines_capasity*sizeof(off_t));
                    lines_lengths = (off_t*)realloc(lines_lengths, lines_capasity*sizeof(off_t));
                    if (lines_shifts == NULL || lines_lengths == NULL) {
                        perror("Memory reallocation failed");
                        return 0;
                    }
                }
            }
        }
        lines_lengths[lines_count] = cur_line_length + 1;
        lines_shifts[lines_count] = total_shift - cur_line_length;
    }
    lines_shifts[0] = 0;
    size_t line_number = 1;
    if(c_char == '\n'){
	lines_count--;
    }
    while(1 == 1) {
        printf("Enter line number:\n");
        if(!scanf("%zd", &line_number)) {
            printf("Wrong line number!\n");
            scanf("%*[^\n]");
            continue;
         }
        if (line_number == 0) {
            break;
        }
        line_number--;
        if (line_number > lines_count) {
            fprintf(stderr,"Wrong line number!\n");
            continue;
        }
        if(lseek(file_desc, lines_shifts[line_number], SEEK_SET) == -1){
            perror("Lseek error!\n");
            return 0;
        }
        off_t line_len = lines_lengths[line_number];
        off_t line_remain = line_len - 1;
        char line_buffer[255];
        printf("Line number %zd:\n", line_number + 1);
        was_readed = 1;
        while(was_readed != 0 && line_remain > 0 ){
            if(line_remain < sizeof(line_buffer)) {
                was_readed = read(file_desc, line_buffer, line_remain);
            }
            else {
                was_readed = read(file_desc, line_buffer, sizeof(line_buffer) - 1);
            }
            if (was_readed == -1) {
                perror("Read error!");
                return 0;
            }
            line_remain -= was_readed;
            line_buffer[was_readed] = '\0';
            printf("%s", line_buffer);
        }
        printf("\n");
    }
    return 0;
}
