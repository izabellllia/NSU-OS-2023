#include "utility.h"
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

extern int errno;

const int index_is_available = -1;
const int all_indices_are_used = -1;

int find_first_not_used_index(const struct pollfd* const connection_descriptors, const int from, const int to, const int step)
{
    if (connection_descriptors == NULL || (from > to && step <= 0) || (from < to && step >= 0))
    {
        fprintf(stderr, "find_first_not_used_index: wrong args\n");
    }

    for (int index = from; index < to; index += step)
    {
        if (connection_descriptors[index].fd == index_is_available)
        {
            return index;
        }
    }

    return all_indices_are_used;
}

in_port_t str_to_in_port_t(const char* const port_as_str, bool* const successfully_converted)
{
    if (port_as_str == NULL || successfully_converted == NULL)
    {
        fprintf(stderr, "str_to_in_port_t: wrong args\n");
    }

    errno = 0;
    const long port_as_long = strtol(port_as_str, NULL, 10);

    if (errno != 0 || port_as_long > MAX_PORT || port_as_long < 0)
    {
        *successfully_converted = false;
        return 0;
    }

    *successfully_converted = true;
    return (in_port_t)port_as_long;
}

char* create_buffer(size_t buffer_size)
{
    if (buffer_size == 0)
    {
        fprintf(stderr, "create_buffer: wrong args\n");
    }

    char* buffer = calloc(buffer_size, sizeof(char));
    if (buffer == NULL)
    {
        perror("buffer allocation");
        exit(-1);
    }
    return buffer;
}

char** create_buffers(const size_t buffers_count, const size_t buffer_size)
{
    if (buffer_size == 0 || buffers_count == 0)
    {
        fprintf(stderr, "create_buffers: wrong args\n");
    }

    char** buffers = calloc(buffers_count, sizeof(char*));
    if (buffers == NULL)
    {
        perror("buffers allocation");
        exit(-1);
    }

    for (size_t buffer_index = 0; buffer_index < buffers_count; ++buffer_index)
    {
        buffers[buffer_index] = create_buffer(buffer_size);
    }

    return buffers;
}

void free_buffers(char** const buffers, const size_t buffers_count, const size_t buffer_size)
{
    if (buffers == NULL || buffer_size == 0 || buffers_count == 0)
    {
        fprintf(stderr, "free_buffers: wrong args\n");
    }

    for (size_t buffer_index = 0; buffer_index < buffers_count; ++buffer_index)
    {
        free(buffers[buffer_index]);
    }
    free(buffers);
}

ssize_t write_and_update_buffer(const int fd, const size_t symbols_count, char* const buffer, 
    char* const auxiliary_buffer, const size_t buffer_size, const char* const failure_msg)
{
    if (fd < 0 || buffer == NULL || auxiliary_buffer == NULL || buffer_size == 0 || failure_msg == NULL)
    {
        fprintf(stderr, "write_and_update_buffer: wrong args\n");
    }

    ssize_t written_symbols = write(fd, buffer, symbols_count);

    if (written_symbols == -1)
    {
        perror(failure_msg);
        exit(-1);
    }

    size_t remained_symbols = symbols_count - (size_t)written_symbols;
    strncpy(auxiliary_buffer, buffer + written_symbols, remained_symbols);
    memset(buffer, 0, buffer_size);
    strncpy(buffer, auxiliary_buffer, remained_symbols);
    memset(auxiliary_buffer, 0, buffer_size);
    return written_symbols;
}

in_port_t get_port(const int argc, char** argv, const size_t port_arg_index, const in_port_t default_port)
{
    if (argv == NULL)
    {
        fprintf(stderr, "get_port: wrong args\n");
    }

    in_port_t port;

    if (argc <= (int)port_arg_index)
    {
        printf("Port was not provided and was initialized on port %ld\n", (long)default_port);
        port = default_port;
    }
    else
    {
        bool port_converted = false;
        port = str_to_in_port_t(argv[port_arg_index], &port_converted);
        if (!port_converted)
        {
            printf("Can not convert port argument to in_port_t.\n");
            exit(-1);
        }
    }

    return port;
}

int create_socket()
{
    int plain_socket;
    if ((plain_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(-1);
    }
    return plain_socket;
}

int create_reuseaddr_socket()
{
    int server_socket;
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Server's socket creation");
        exit(-1);
    }

    int reuseaddr_flag = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &reuseaddr_flag, sizeof(int)) == -1)
    {
        perror("setsockopt SO_REUSEADDR for socket");
        exit(-1);
    }

    return server_socket;
}

void set_sndbuf(const int socket, const int sndbuf)
{
    if (setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(int)) == -1)
    {
        perror("setsockopt SO_SNDBUF for socket");
        exit(-1);
    }
}

void bind_and_listen(const int server_socket, const in_port_t server_port, const size_t clients_number)
{
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(server_port);
    if (bind(server_socket, (const struct sockaddr*)&server_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("bind");
        exit(-1);
    }

    if (listen(server_socket, clients_number) != 0)
    {
        perror("listen");
        exit(-1);
    }
}
