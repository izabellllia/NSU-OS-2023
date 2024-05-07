#include "utility.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>

char* create_buffer(size_t buffer_size)
{
    if (buffer_size == 0)
    {
        fprintf(stderr, "create_buffer: zero buffer_size\n");
        exit(-1);
    }

    char* buffer = calloc(buffer_size, sizeof(char));
    if (buffer == NULL)
    {
        perror("buffer allocation");
        exit(-1);
    }
    return buffer;
}

int create_socket()
{

    int plain_socket;
    if ((plain_socket = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        perror("UNIX domain socket creation");
        exit(-1);
    }
    return plain_socket;
}
