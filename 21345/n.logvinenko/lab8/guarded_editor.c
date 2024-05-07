#include <stdio.h>
#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

static const size_t length_of_space = 1;
static const size_t terminating_zero_byte = 1;

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Provide an editor and a path to a file that will be openned with editor");
        return -1;
    }

    const char* editor = argv[1];
    const char* filename = argv[2];
    int fd;
    if ((fd = open(filename, O_RDWR)) == -1)
    {
        fprintf(stderr, "open %s: ", filename);
        perror(NULL);
        return -1;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1)
    {
        perror("fcntl, attempt to lock the file");
        return -1;
    }

    size_t command_length = strlen(editor) + length_of_space + strlen(filename) + terminating_zero_byte;
    char* command = (char*)malloc(command_length);
    if (command == NULL)
    {
        perror("malloc buffer for command");
        return -1;
    }
    sprintf(command, "%s %s", editor, filename);

    int system_result = system(command);
    if (system_result == -1)
    {
        perror("system");
        return -1;
    }
    else if (WIFEXITED(system_result) && WEXITSTATUS(system_result) == 127)
    {
        fprintf(stderr, "system can not execute 'shell' in the child process.\n");
        return -1;
    }

    lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) == -1)
    {
        perror("fcntl, attempt to unlock the file");
        return -1;
    }

    return 0;
}
