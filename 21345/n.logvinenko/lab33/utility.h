#ifndef UTILITY
#define UTILITY

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>

#define IN_PORT_T_BITS (CHAR_BIT * sizeof(in_port_t))
#define MAX_PORT ((1 << IN_PORT_T_BITS) - 1)

extern const int index_is_available;
extern const int all_indices_are_used;

int find_first_not_used_index(const struct pollfd* connection_descriptors, int from, int to, int step);

in_port_t str_to_in_port_t(const char* port_as_str, bool* successfully_converted);

char* create_buffer(size_t buffer_size);

char** create_buffers(size_t buffers_count, size_t buffer_size);

void free_buffers(char** buffers, size_t buffers_count, size_t buffer_size);

ssize_t write_and_update_buffer(int fd, size_t symbols_count, char* buffer, char* auxiliary_buffer, size_t buffer_size, const char* failure_msg);

in_port_t get_port(int argc, char** argv, size_t port_arg_index, in_port_t default_port);

int create_socket();

int create_reuseaddr_socket();

void set_sndbuf(int socket, int sndbuf);

void bind_and_listen(int server_socket, in_port_t server_port, size_t clients_number);

#endif
