#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#define SIZE 512

void printUpper(char* buffer, ssize_t availableAmount)
{
    for(ssize_t counter =0; counter < availableAmount; counter++)
    { 
         printf("%c", toupper(*buffer++));
    }
}

void main(int argc, char* argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "No file\n");
        exit(0);
    }

    int f_d;

    if((f_d = open(argv[1], O_RDONLY)) == -1)
    {
        perror("Cant open file");
        exit(1);
    }

    int fd[2];

    if(pipe(fd) == -1)
    {
        perror("Can't create pipe");
        exit(1);
    }

    pid_t child;

    if((child = fork()) == -1)
    {
        perror("Can't fork");
        exit(1);
    }

    else if(child > 0)
    {
        close(fd[0]);
        char outputBuf[SIZE];
        ssize_t readFromFileSize;
        while((readFromFileSize = read(f_d, outputBuf, SIZE)) != 0)
        {
             write(fd[1], outputBuf, readFromFileSize);
        }
        close(fd[1]);
        wait(0);
        close(f_d);
    }

    else
    {
        close(fd[1]);
        char inputBuf[SIZE];
        ssize_t readFromPipeSize;
        while((readFromPipeSize = read(fd[0], inputBuf, SIZE))!= 0)
        {
            printUpper(inputBuf, readFromPipeSize);
        }
    }
    exit(0);
}
