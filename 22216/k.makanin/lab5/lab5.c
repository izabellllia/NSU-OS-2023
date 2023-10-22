#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE_OF_BUFFER 1024


typedef struct line {
    int offset;
    int len;
} line;


void addMemory(int *extension, int numberOfLine, line *temp, line *lines) {
    if ((*extension) < numberOfLine){
        (*extension) *= 2;
        temp = realloc(lines, (*extension) * sizeof(line));
        if (temp  != NULL){
            lines = temp;
        }
        else{
            perror("Realloc failed");
        }
    }
}


void printLine(line *lines, int fd, int choice) {
    int lenStr = lines[choice - 1].len;
    char strOut[lenStr];
    if (lseek(fd, lines[choice - 1].offset, SEEK_SET) != lines[choice - 1].offset) {
        perror("Not correct number of bytes.");
    };
    if (read(fd, strOut, lenStr) == -1){
        perror("Cannot read line.");
    };
    printf("%.*s\n", lenStr, strOut);
}


int main() {

    int fd = open("./text.txt", O_RDONLY);
    if (fd == -1)
    {
        perror("Opening failed.");
        return -1;
    }
    
    int lenStr = 0,
        posInBuffer = 0,
        numberOfLine = 0,
        extension = 1,
        previousLen = 0;
    
    char choice[100];
    
    char buffer[SIZE_OF_BUFFER];

    line *temp = NULL;
    line *lines;
    lines = (line*) malloc(sizeof(line));
    if ( lines == NULL)
    {
        perror("Creating malloc failed");
        return -1;
    }
    
    
    ssize_t bytesRead;
    bytesRead = read(fd, &buffer, SIZE_OF_BUFFER);
    

    while (bytesRead > 0)
    {
        while (buffer[posInBuffer] != '\n' && bytesRead > 0)
        {
            bytesRead--;
            lenStr++;
            posInBuffer++;
        }
        
        if (buffer[posInBuffer] == '\n')
        {
            addMemory(&extension, numberOfLine, temp, lines);
            lines[numberOfLine].offset = previousLen + posInBuffer - lenStr;
            lines[numberOfLine].len = lenStr;

            lenStr = 0;
            numberOfLine++;
            posInBuffer++;
            bytesRead--;
        }
        
        if (bytesRead == 0)
        {
            previousLen += posInBuffer;
            posInBuffer = 0;
            bytesRead = read(fd, &buffer, SIZE_OF_BUFFER);
        }
    }

    
    do
    {
        printf("Choose the line:\n");
        scanf("%s", choice);
        while (atol(choice) < 0 || atol(choice) == 0 && choice[0] != '0'){
            printf("You need choose a positive number:\n");
            scanf("%s", choice);
        }
        
        if (choice[0] == '0'){
            printf("End of work.");
            break;
        }
        
        if (atol(choice) > numberOfLine){
            printf("There are fewer lines in the file.\n");
        }
        else{
            printLine(lines, fd, atol(choice));
        }
    }
    while (choice != 0);

    
    free(lines);
    free(temp);
    close(fd);
}
