#include "utility.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

#define CLIENTS_NUMBER 510
#define TOTAL_SOCKETS_NUMBER (CLIENTS_NUMBER+1)

static const size_t server_port_arg = 1;
static const in_port_t default_server_port = 8888;
static const int poll_wait = 1000;
static const int server_index = 0;
static const size_t buffer_size = BUFSIZ + 1;

static void init_pollfd(struct pollfd* const connection_descriptors, const size_t count, const int server_socket)
{
    if (connection_descriptors == NULL || server_index >= count)
    {
        fprintf(stderr, "init_pollfd: invalid params.\n");
        exit(-1);
    }

    for (size_t index = 0; index < count; index++)
    {
        connection_descriptors[index].fd = index_is_available;
        connection_descriptors[index].events = POLLIN;
    }
    connection_descriptors[server_index].fd = server_socket;
}

static void accept_new_client(const int server_socket, struct pollfd* const connection_descriptors)
{
    if (connection_descriptors == NULL)
    {
        fprintf(stderr, "accept_new_client: invalid params.\n");
        exit(-1);
    }

    int new_client_socket;
    if ((new_client_socket = accept(server_socket, NULL, NULL)) == -1)
    {
        perror("Server: accept");
        exit(-1);
    }
    set_sndbuf(new_client_socket, buffer_size - 1);

    int new_client_index = 0;
    if ((new_client_index = find_first_not_used_index(connection_descriptors, server_index + 1, TOTAL_SOCKETS_NUMBER, 1)) == all_indices_are_used)
    {
        close(new_client_socket);
    }
    else
    {
        connection_descriptors[new_client_index].fd = new_client_socket;
        printf("Server: client %d was connected!\n", new_client_index);
    }
}

static void close_connection(const int client_index, struct pollfd* const connection_descriptors, char* const buffer, const size_t buffer_size)
{
    if (client_index <= server_index)
    {
        fprintf(stderr, "close_connection: invalid params.\n");
        exit(-1);
    }

    printf("Server: client %d closed connection\n", client_index);
    close(connection_descriptors[client_index].fd);
    connection_descriptors[client_index].fd = index_is_available;
    connection_descriptors[client_index].events = POLLIN;
    memset(buffer, 0, buffer_size);
}

static void handle_client(const int client_index, struct pollfd* const connection_descriptors,
    char* const buffer, char* const auxiliary_buffer, const size_t buffer_size)
{
    if (client_index <= server_index || buffer_size <= 1 || buffer == NULL || connection_descriptors == NULL)
    {
        fprintf(stderr, "handle_client: invalid params.\n");
        exit(-1);
    }

    if ((connection_descriptors[client_index].revents & POLLIN) && (connection_descriptors[client_index].events & POLLIN))
    {
        ssize_t read_symbols = read(connection_descriptors[client_index].fd, buffer, buffer_size - 1);

        if (read_symbols == -1)
        {
            perror("Server read client's msg");
            exit(-1);
        }

        if (read_symbols == 0)
        {
            close_connection(client_index, connection_descriptors, buffer, buffer_size);
            return;
        }

        printf("Server: client %d sent %d symbols: %s\n", client_index, (int)read_symbols, buffer);
        connection_descriptors[client_index].events = POLLOUT;
        return;
    }

    if ((connection_descriptors[client_index].revents & POLLOUT) && (connection_descriptors[client_index].events & POLLOUT))
    {
        const size_t read_symbols = strlen(buffer) + 1;
        ssize_t written_symbols = write_and_update_buffer(connection_descriptors[client_index].fd, read_symbols, buffer,
            auxiliary_buffer, buffer_size, "Server: writing failed");

        if (written_symbols == 0)
        {
            close_connection(client_index, connection_descriptors, buffer, buffer_size);
            return;
        }

        if (read_symbols - written_symbols == 0)
        {
            printf("Server: sent message back to client %d\n", client_index);
            connection_descriptors[client_index].events = POLLIN;
            return;
        }
    }
}


int main(int argc, char** argv)
{
    if (sigset(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        perror("Can not set handler for SIGPIPE");
        return -1;
    }

    const in_port_t server_port = get_port(argc, argv, server_port_arg, default_server_port);
    const int server_socket = create_reuseaddr_socket();
    bind_and_listen(server_socket, server_port, CLIENTS_NUMBER);

    struct pollfd connection_descriptors[TOTAL_SOCKETS_NUMBER];
    init_pollfd(connection_descriptors, TOTAL_SOCKETS_NUMBER, server_socket);

    char** buffers = create_buffers(CLIENTS_NUMBER, buffer_size);
    char* auxiliary_buffer = create_buffer(buffer_size);
    int polled_descrs_n = 0;
    while(true)
    {
        if ((polled_descrs_n = poll(connection_descriptors, TOTAL_SOCKETS_NUMBER, poll_wait)) == -1)
        {
            perror("Server: poll");
            return -1;
        }

        if (polled_descrs_n == 0)
        {
            continue;
        }

        if (connection_descriptors[server_index].revents & POLLIN)
        {
            accept_new_client(server_socket, connection_descriptors);
        }

        for (int client_index = server_index + 1; client_index < TOTAL_SOCKETS_NUMBER; client_index++)
        {
            handle_client(client_index, connection_descriptors, buffers[client_index - server_index - 1], auxiliary_buffer, buffer_size);
        }
    }
}
