#include "utility.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>

static const size_t buffer_size = BUFSIZ;
static bool connection_is_active = false;

static void sigpipe_handler(int signo)
{
    connection_is_active = false;
}

static void connect_to_dest(const int dest_socket, const char* const socket_path)
{
    if (socket_path == NULL)
    {
        fprintf(stderr, "connect_to_dest: socket_path is NULL.\n");
        exit(-1);
    }

    struct sockaddr_un socket_addr;
    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sun_family = AF_UNIX;
    strcpy(socket_addr.sun_path, socket_path);

    if (connect(dest_socket, (struct sockaddr*)&socket_addr, sizeof(socket_addr)) == -1) 
    {
        perror("Connection error");
        close(dest_socket);
        exit(-1);
    }

    connection_is_active = true;
}

static void send_request_for_msg()
{
    printf("Enter message for server: ");
    fflush(stdout);
}

static void free_buffers(char* buffer, char* auxiliary_buffer)
{
    free(buffer);
    free(auxiliary_buffer);
}

static ssize_t write_and_update_buffer(const int fd, const size_t symbols_count, char* const buffer,
    char* const auxiliary_buffer, const size_t buffer_size, const char* const failure_msg)
{
    if (buffer == NULL || auxiliary_buffer == NULL || buffer_size == 0 || failure_msg == NULL)
    {
        fprintf(stderr, "write_and_update_buffer: wrong args\n");
        exit(-1);
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

static void write_to_dest(const int dest_socket, const ssize_t symbols_count, char* buffer, char* auxiliary_buffer, const size_t buffer_size)
{
    if (buffer == NULL || auxiliary_buffer == NULL || buffer_size == 0)
    {
        fprintf(stderr, "write_to_dest: invalid arguments were passed.\n");
        exit(-1);
    }

    ssize_t written_symbols = write_and_update_buffer(dest_socket, (size_t)symbols_count, buffer, auxiliary_buffer, buffer_size, "Client: writing failed");
    size_t remained_symbols = symbols_count - (size_t)written_symbols;
    while (remained_symbols != 0)
    {
        if (written_symbols == 0)
        {
            printf("Connection with server was closed!\n");
            free_buffers(buffer, auxiliary_buffer);
            exit(0);
        }

        written_symbols = write_and_update_buffer(dest_socket, remained_symbols, buffer, auxiliary_buffer, buffer_size, "Client: writing failed");
        remained_symbols -= (size_t)written_symbols;
    }
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Provide a path for UNIX domain socket.\n");
        return -1;
    }
    const char* const socket_path = argv[1];

    if (sigset(SIGPIPE, sigpipe_handler) == SIG_ERR)
    {
        perror("Can not set handler for SIGPIPE");
        return -1;
    }

    const int dest_socket = create_socket();
    connect_to_dest(dest_socket, socket_path);

    char* buffer = create_buffer(buffer_size);
    char* auxiliary_buffer = create_buffer(buffer_size);
    ssize_t read_symbols;
    send_request_for_msg();

    while ((read_symbols = read(STDIN_FILENO, buffer, buffer_size)) > 0)
    {
        if (!connection_is_active)
        {
            printf("Connection with server was closed!\n");
            free_buffers(buffer, auxiliary_buffer);
            return 0;
        }

        write_to_dest(dest_socket, read_symbols, buffer, auxiliary_buffer, buffer_size);
        send_request_for_msg();
    }

    if (read_symbols == -1)
    {
        perror("Reading from stdin failed");
        free_buffers(buffer, auxiliary_buffer);
        return -1;
    }

    free_buffers(buffer, auxiliary_buffer);
    return 0;
}
