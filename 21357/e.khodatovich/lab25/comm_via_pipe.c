#define SIZE 512

#include <stdlib.h> // exit, EXIT_FAILURE, EXIT_SUCCESS
#include <fcntl.h> // open()
#include <unistd.h> // write(), read(), close() 
#include <stdio.h> // perror(), printf()
#include <ctype.h> // toupper()
char separator [21] = "====================\n";
int main (int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Wrong number of arguments. Allowed number: 1\n");
        exit(EXIT_FAILURE);
    }
    int fd[2];
    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit (EXIT_FAILURE);
    }
    pid_t child_pid = fork();
    switch (child_pid)
    {
        case -1:
            perror("fork");
            exit (EXIT_FAILURE);
        case 0:
            if(close(fd[0]))
            {
                perror("child hasn't closed write-pipe");
                exit(EXIT_FAILURE);
            }
            char readbuf[SIZE];
            ssize_t read_sym_number;
            printf("processed text\n%s", separator);
            while ((read_sym_number= read(fd[1], readbuf, SIZE)) != 0)
            {
                if (read_sym_number == -1)
                {
                    perror("child couldn't read");
                    exit (EXIT_FAILURE);
                }
                int i = 0;
                for (i; i<read_sym_number; ++i)
                {
                    readbuf[i] = toupper(readbuf[i]);
                }
                printf("%.*s", read_sym_number, readbuf);
            }
            printf("\n");
            if (close(fd[1]))
            {
                perror ("child hasn't closed read-pipe");
                exit(EXIT_FAILURE);
            }
            exit(EXIT_SUCCESS);
    
        default:
            if (close(fd[1]))
            {
                perror("parent hasn't closed read-pipe");
                exit(EXIT_FAILURE);
            }
            int file_handle = open(argv[1], O_RDONLY);
            if (file_handle == -1) 
            {
                perror("parent hasn't read the file");
                exit(EXIT_FAILURE);
            }

            char string[SIZE];
            int bytes_read;
            while ((bytes_read=read(file_handle, string, SIZE)) != 0)
            {
                if (bytes_read == -1)
                {
                    perror("parent couldn't read from file");
                    exit(EXIT_FAILURE);
                }
                write (fd[0], string, bytes_read);
            }

            if (close(fd[0]))
            {
                perror("parent hasn't closed write-pipe");
                exit(EXIT_FAILURE);
            }
            exit (EXIT_SUCCESS);
    }
}
