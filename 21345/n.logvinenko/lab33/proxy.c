#include "utility.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <signal.h>

#define PROXIES_NUMBER 1
#define CLIENTS_NUMBER 510
#define DEST_SOCKETS_NUMBER CLIENTS_NUMBER
#define TOTAL_NON_PROXY_SOCKETS (CLIENTS_NUMBER + DEST_SOCKETS_NUMBER)
#define POLLFDS_BY_CLIENT 2
#define POLLFDS_COUNT (PROXIES_NUMBER + CLIENTS_NUMBER * POLLFDS_BY_CLIENT)

enum offset_in_connection
{
    client_offset,
    dest_offset
};

static const int total_args = 4;
static const size_t source_port_arg = 1;
static const size_t dest_port_arg = 2;
static const size_t host_arg = 3;
static const int poll_wait = -1;
static const size_t proxy_index = 0;
static const size_t buffer_size = BUFSIZ;
static const in_port_t source_port_default = 8000;
static const in_port_t dest_port_default = 8888;

int pollfd_index_for_new_client(const struct pollfd* const connection_descriptors)
{
    if (connection_descriptors == NULL)
    {
        fprintf(stderr, "pollfd_index_for_new_client: connection_descriptors == NULL.\n");
    }

    for (int index = proxy_index + 1; index < POLLFDS_COUNT; index += POLLFDS_BY_CLIENT)
    {
        if (connection_descriptors[index].fd == index_is_available)
        {
            return index;
        }
    }

    return all_indices_are_used;
}

size_t pollfd_index_to_connection_index(const size_t pollfd_index)
{
    if (pollfd_index <= proxy_index)
    {
        fprintf(stderr, "pollfd_index_to_connection_index: pollfd_index must be greater than proxy_index.\n");
        exit(-1);
    }

    return (pollfd_index - proxy_index - 1) / POLLFDS_BY_CLIENT;
}

static struct in_addr get_host_in_addr(const int argc, char** argv, const size_t host_arg_index, const size_t dest_port_arg_index)
{
    if (host_arg_index >= argc || dest_port_arg_index >= argc || argv == NULL)
    {
        fprintf(stderr, "get_host_in_addr, invalid params.\n");
        exit(-1);
    }

    struct addrinfo host_addr_hints;
    memset(&host_addr_hints, 0, sizeof(host_addr_hints));
    host_addr_hints.ai_family = AF_INET;
    host_addr_hints.ai_socktype = SOCK_STREAM;
    host_addr_hints.ai_protocol = 0;
    host_addr_hints.ai_flags = AI_NUMERICSERV;

    struct addrinfo* host_addr_result;
    const int getaddr_return = getaddrinfo(argv[host_arg_index], argv[dest_port_arg_index], &host_addr_hints, &host_addr_result);
    if (getaddr_return != 0)
    {
        fprintf(stderr, "Proxy: getaddrinfo: %s\n", gai_strerror(getaddr_return));
        exit(-1);
    }
    if (host_addr_result == NULL || host_addr_result->ai_addr == NULL)
    {
        fprintf(stderr, "Proxy: can not find host: %s\n", argv[host_arg]);
        exit(-1);
    }
    const struct in_addr host_in_addr = ((struct sockaddr_in*)(host_addr_result->ai_addr))->sin_addr;
    freeaddrinfo(host_addr_result);
    return host_in_addr;
}

static void set_sockets_to_pollfds(struct pollfd* const pollfd_1, struct pollfd* const pollfd_2, const int socket_1, const int socket_2)
{
    if (pollfd_1 == NULL || pollfd_2 == NULL)
    {
        fprintf(stderr, "clear_pollfds_for_one_client: invalid params.\n");
        exit(-1);
    }

    pollfd_1->fd = socket_1;
    pollfd_2->fd = socket_2;
}

static void reset_revents(struct pollfd* const pollfd_to_reset)
{
    if (pollfd_to_reset == NULL)
    {
        fprintf(stderr, "reset_revents: invalid params.\n");
        exit(-1);
    }

    memset(&(pollfd_to_reset->revents), 0, sizeof(pollfd_to_reset->revents));
}

static void reset_pollfds(struct pollfd* const pollfd_1, struct pollfd* const pollfd_2)
{
    if (pollfd_1 == NULL || pollfd_2 == NULL)
    {
        fprintf(stderr, "reset_pollfds: invalid params.\n");
        exit(-1);
    }

    set_sockets_to_pollfds(pollfd_1, pollfd_2, index_is_available, index_is_available);

    pollfd_1->events = POLLIN;
    pollfd_2->events = POLLIN;

    reset_revents(pollfd_1);
    reset_revents(pollfd_2);
}

static void init_pollfd(struct pollfd* const connection_descriptors, const size_t count, const int proxy_socket)
{
    if (connection_descriptors == NULL || proxy_index >= count)
    {
        fprintf(stderr, "init_pollfd: invalid params.\n");
        exit(-1);
    }

    for (size_t pollfd_index = proxy_index + 1; pollfd_index < count; pollfd_index += POLLFDS_BY_CLIENT)
    {
        reset_pollfds(connection_descriptors + pollfd_index + client_offset, connection_descriptors + pollfd_index + dest_offset);
    }

    connection_descriptors[proxy_index].fd = proxy_socket;
    connection_descriptors[proxy_index].events = POLLIN;
}

static void translate_new_client(const int proxy_socket, struct pollfd* const connection_descriptors, const int sndbuf,
    const struct in_addr host_in_addr, const in_port_t dest_port)
{
    if (connection_descriptors == NULL)
    {
        fprintf(stderr, "translate_new_client, invalid params.\n");
        exit(-1);
    }

    int new_pollfd_index;
    if ((new_pollfd_index = pollfd_index_for_new_client(connection_descriptors)) == all_indices_are_used)
    {
        fprintf(stderr, "Max possible connections were reached. Can not accept one more client.\n");
        return;
    }

    int new_client_socket;
    if ((new_client_socket = accept(proxy_socket, NULL, NULL)) == -1)
    {
        perror("Proxy: accept");
        return;
    }
    set_sndbuf(new_client_socket, sndbuf);

    printf("Proxy: client %d accepted\n", (int)pollfd_index_to_connection_index((size_t)new_pollfd_index));

    int dest_socket;
    if ((dest_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Proxy: destination socket");
        close(new_client_socket);
        return;
    }
    set_sndbuf(dest_socket, sndbuf);

    struct sockaddr_in dest_addr;
    memset(&dest_addr, 0, sizeof(struct sockaddr_in));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_addr = host_in_addr;
    dest_addr.sin_port = htons(dest_port);

    if (connect(dest_socket, (const struct sockaddr*)&dest_addr, sizeof(struct sockaddr_in)) == -1)
    {
        perror("Proxy: destination connect");
        close(new_client_socket);
        close(dest_socket);
        return;
    }

    set_sockets_to_pollfds(connection_descriptors + (size_t)new_pollfd_index + client_offset, 
        connection_descriptors + (size_t)new_pollfd_index + dest_offset, new_client_socket, dest_socket);

    printf("Proxy: new connection was established succesfully!\n");
}

static void close_connection(const int connection_index, struct pollfd* const pollfd_1, struct pollfd* const pollfd_2,
    char* const buffer_1, char* const buffer_2, const size_t buffer_size,
    size_t* const not_written_1, size_t* const not_written_2)
{
    if (pollfd_1 == NULL || pollfd_2 == NULL || buffer_1 == NULL || buffer_2 == NULL || not_written_1 == NULL || not_written_2 == NULL)
    {
        fprintf(stderr, "close_connection: invalid params.\n");
        exit(-1);
    }

    close(pollfd_1->fd);
    close(pollfd_2->fd);

    reset_pollfds(pollfd_1, pollfd_2);

    memset(buffer_1, 0, buffer_size);
    memset(buffer_2, 0, buffer_size);
    *not_written_1 = 0;
    *not_written_2 = 0;

    printf("Proxy: connection %d was closed\n", connection_index);
}

static void handle_socket(const int connection_index, struct pollfd* const handled_pollfd, struct pollfd* const paired_pollfd,
    char* const buffer_for_sending, char* const buffer_for_receiving, char* const auxiliary_buffer, const size_t buffer_size,
    size_t* const not_sent_symbols, size_t* const not_received_symbols)
{
    if (buffer_size == 0 || buffer_for_receiving == NULL || buffer_for_sending == NULL || auxiliary_buffer == NULL ||
        handled_pollfd == NULL || paired_pollfd == NULL || not_sent_symbols == NULL || not_received_symbols == NULL)
    {
        fprintf(stderr, "handle_socket: invalid params.\n");
        exit(-1);
    }

    if (handled_pollfd->revents & POLLIN)
    {
        ssize_t read_symbols = read(handled_pollfd->fd, buffer_for_sending, buffer_size);
        if (read_symbols == -1)
        {
            perror("Proxy read msg");
            exit(-1);
        }

        if (read_symbols == 0)
        {
            close_connection(connection_index, handled_pollfd, paired_pollfd,
                buffer_for_receiving, buffer_for_sending, buffer_size, not_sent_symbols, not_received_symbols);
        }

        else
        {
            *not_sent_symbols = (size_t)read_symbols;
            handled_pollfd->events &= ~POLLIN;
            handled_pollfd->revents &= ~POLLIN;
            paired_pollfd->events |= POLLOUT;
        }
    }

    if (handled_pollfd->revents & POLLOUT)
    {
        ssize_t written_symbols = write_and_update_buffer(handled_pollfd->fd, *not_received_symbols,
            buffer_for_receiving, auxiliary_buffer, buffer_size, "Proxy translate msg");

        if (written_symbols == 0)
        {
            close_connection(connection_index, handled_pollfd, paired_pollfd,
                buffer_for_receiving, buffer_for_sending, buffer_size, not_sent_symbols, not_received_symbols);
        }

        else
        {
            *not_received_symbols -= (size_t)written_symbols;
            if (*not_received_symbols == 0)
            {
                handled_pollfd->events &= ~POLLOUT;
                handled_pollfd->revents &= ~POLLOUT;
                paired_pollfd->events |= POLLIN;
            }
        }
    }
}


int main(int argc, char* argv[])
{
    if (argc != total_args)
    {
        printf("Proxy: program expects arguments:\n1. source port\n2: destination port\n3: host\n");
        return -1;
    }

    if (sigset(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        perror("Can not set handler for SIGPIPE");
        return -1;
    }

    const in_port_t source_port = get_port(argc, argv, source_port_arg, source_port_default);
    const in_port_t dest_port = get_port(argc, argv, dest_port_arg, dest_port_default);
    const struct in_addr host_in_addr = get_host_in_addr(argc, argv, host_arg, dest_port_arg);

    const int proxy_socket = create_reuseaddr_socket();
    bind_and_listen(proxy_socket, source_port, CLIENTS_NUMBER);

    struct pollfd connection_descriptors[POLLFDS_COUNT];
    init_pollfd(connection_descriptors, POLLFDS_COUNT, proxy_socket);

    char** buffers = create_buffers(TOTAL_NON_PROXY_SOCKETS, buffer_size);
    size_t* not_written_symbols = (size_t*)(void*)create_buffer(TOTAL_NON_PROXY_SOCKETS * sizeof(size_t));
    char* auxiliary_buffer = create_buffer(buffer_size);
    int polled_descrs_n = 0;
    while(true)
    {
        if ((polled_descrs_n = poll(connection_descriptors, POLLFDS_COUNT, poll_wait)) == -1)
        {
            perror("Proxy: poll");
            return -1;
        }

        if (polled_descrs_n == 0)
        {
            continue;
        }

        if (connection_descriptors[proxy_index].revents & POLLIN)
        {
            translate_new_client(proxy_socket, connection_descriptors, buffer_size, host_in_addr, dest_port);
        }

        for (size_t pollfd_index = proxy_index + 1; pollfd_index < POLLFDS_COUNT; pollfd_index += POLLFDS_BY_CLIENT)
        {
            const size_t connection_index = pollfd_index_to_connection_index(pollfd_index);
            const size_t client_to_dest_buffer_index = connection_index * 2 + client_offset;
            const size_t dest_to_client_buffer_index = connection_index * 2 + dest_offset;

            struct pollfd* const pollfds_for_client = connection_descriptors + pollfd_index;

            handle_socket(connection_index, pollfds_for_client + client_offset, pollfds_for_client + dest_offset,
                buffers[client_to_dest_buffer_index], buffers[dest_to_client_buffer_index], auxiliary_buffer, buffer_size,
                not_written_symbols + client_to_dest_buffer_index, not_written_symbols + dest_to_client_buffer_index);

            handle_socket(connection_index, pollfds_for_client + dest_offset, pollfds_for_client + client_offset,
                buffers[dest_to_client_buffer_index], buffers[client_to_dest_buffer_index], auxiliary_buffer, buffer_size,
                not_written_symbols + dest_to_client_buffer_index, not_written_symbols + client_to_dest_buffer_index);
        }
    }
}
