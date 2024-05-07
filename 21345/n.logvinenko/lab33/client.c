#include "utility.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

static const size_t dest_port_arg = 1;
static const in_port_t default_dest_port = 8000;
static const size_t buffer_size = BUFSIZ + 1;

static bool connection_is_active = false;

static void sigpipe_handler(int signo)
{
    connection_is_active = false;
}

static void connect_to_loopback(const int dest_socket, const in_port_t dest_port)
{
    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(struct sockaddr_in));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    dest_addr.sin_port = htons(dest_port);

    if (connect(dest_socket, (const struct sockaddr*)&dest_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Client: connection error");
        exit(-1);
    }
    else
    {
        printf("Client: connected!\n");
        connection_is_active = true;
    }
}

static void send_request_for_msg()
{
    printf("Client pid=%d, enter message for server: ", (int)getpid());
    fflush(stdout);
}

static void _free_buffers(char* buffer, char* auxiliary_buffer)
{
    free(buffer);
    free(auxiliary_buffer);
}

static void write_to_dest(const int dest_socket, const ssize_t read_symbols, char* buffer, char* auxiliary_buffer, const size_t buffer_size)
{
    if (buffer == NULL || auxiliary_buffer == NULL || buffer_size <= 1)
    {
        fprintf(stderr, "write_to_dest: invalid arguments were passed.\n");
        exit(-1);
    }

    ssize_t written_symbols = write_and_update_buffer(dest_socket, (size_t)read_symbols + 1, buffer, auxiliary_buffer, buffer_size, "Client: writing failed");
    size_t remained_symbols = read_symbols + 1 - (size_t)written_symbols;
    while (remained_symbols != 0)
    {
        if (written_symbols == 0)
        {
            printf("Client pid=%d, connection with server was closed!\n", (int)getpid());
            _free_buffers(buffer, auxiliary_buffer);
            exit(0);
        }

        written_symbols = write_and_update_buffer(dest_socket, remained_symbols, buffer, auxiliary_buffer, buffer_size, "Client: writing failed");
        remained_symbols -= (size_t)written_symbols;
    }
}

static void receive_from_dest(const int dest_socket, char* buffer, char* auxiliary_buffer, const size_t buffer_size)
{
    if (buffer == NULL || auxiliary_buffer == NULL)
    {
        fprintf(stderr, "receive_from_dest: invalid ptr.\n");
        exit(-1);
    }

    if (read(dest_socket, buffer, buffer_size - 1) == -1)
    {
        perror("Client: reading from socket failed");
        _free_buffers(buffer, auxiliary_buffer);
        exit(-1);
    }

    printf("Server answered: %s\n", buffer);
    memset(buffer, 0, buffer_size);
}


int main(int argc, char** argv)
{
    if (sigset(SIGPIPE, sigpipe_handler) == SIG_ERR)
    {
        perror("Can not set handler for SIGPIPE");
        return -1;
    }

    const in_port_t dest_port = get_port(argc, argv, dest_port_arg, default_dest_port);
    const int dest_socket = create_socket();
    connect_to_loopback(dest_socket, dest_port);

    char* buffer = create_buffer(buffer_size);
    char* auxiliary_buffer = create_buffer(buffer_size);
    ssize_t read_symbols;
    send_request_for_msg();
    while ((read_symbols = read(STDIN_FILENO, buffer, buffer_size - 1)) > 0)
    {
        if (!connection_is_active)
        {
            printf("Client pid=%d, connection with server was closed!\n", (int)getpid());
            _free_buffers(buffer, auxiliary_buffer);
            return 0;
        }

        write_to_dest(dest_socket, read_symbols, buffer, auxiliary_buffer, buffer_size);
        receive_from_dest(dest_socket, buffer, auxiliary_buffer, buffer_size);

        send_request_for_msg();
    }

    if (read_symbols == -1)
    {
        perror("Client: reading from stdin failed");
        _free_buffers(buffer, auxiliary_buffer);
        return -1;
    }

    _free_buffers(buffer, auxiliary_buffer);
    return 0;
}
