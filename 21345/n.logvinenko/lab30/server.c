#include "utility.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

static const size_t buffer_size = BUFSIZ;
static const char* socket_path = NULL;
static bool need_to_unlink = false;

static void unlink_socket_at_exit()
{
    if (socket_path != NULL && need_to_unlink)
    {
        if (unlink(socket_path) != 0)
        {
            perror("unlink socket");
        }
    }
}

static void sigcatch(int sig)
{
	if (sig == SIGINT && socket_path != NULL && need_to_unlink)
	{
        if (unlink(socket_path) != 0)
        {
            perror("unlink socket");
        }
	}
}

static void print_upper_case(const char* string, const size_t symbols_count)
{
    if (string == NULL)
    {
        fprintf(stderr, "print_upper_case: NULL string was passed.\n");
        exit(-1);
    }

    for (size_t symbol_index = 0; symbol_index < symbols_count; ++symbol_index)
    {
        putchar(toupper(*string++));
    }
}

static int bind_listen_accept(const int server_socket, const char* const socket_path)
{
    if (socket_path == NULL)
    {
        fprintf(stderr, "bind_and_listen: socket_path is NULL.\n");
        exit(-1);
    }

    struct sockaddr_un socket_addr;
    memset(&socket_addr, 0, sizeof(socket_addr));
    socket_addr.sun_family = AF_UNIX;
    strcpy(socket_addr.sun_path, socket_path);

    if (bind(server_socket, (const struct sockaddr*)&socket_addr, sizeof(socket_addr)) == -1)
    {
        perror("bind");
        exit(-1);
    }
    need_to_unlink = true;

    if (listen(server_socket, 1) != 0)
    {
        perror("listen");
        exit(-1);
    }

    const int connection_socket = accept(server_socket, NULL, NULL);
    if (connection_socket == -1)
    {
        perror("accept");
        exit(-1);
    }

    need_to_unlink = false;
    if (unlink(socket_path) != 0)
    {
        perror("unlink socket");
        exit(-1);
    }

    return connection_socket;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Provide a path for UNIX domain socket.\n");
        return -1;
    }
    socket_path = argv[1];

    if (atexit(unlink_socket_at_exit) != 0)
    {
        fprintf(stderr, "Can not set atexit.\n");
        return -1;
    }
    if (sigset(SIGINT, sigcatch) == SIG_ERR)
    {
        perror("Can not set SIGINT handler");
        exit(-1);
    }

    if (sigset(SIGPIPE, SIG_IGN) == SIG_ERR)
    {
        perror("Can not set handler for SIGPIPE");
        return -1;
    }

    const int server_socket = create_socket();
    const int connection_socket = bind_listen_accept(server_socket, socket_path);

    char* buffer = create_buffer(buffer_size);
    ssize_t read_symbols;
    while ((read_symbols = read(connection_socket, buffer, buffer_size)) > 0)
    {
        print_upper_case(buffer, read_symbols);
    }

    if (read_symbols == -1)
    {
        perror("Reading from socket failed");
        free(buffer);
        return -1;
    }

    free(buffer);
    return 0;
}
