#include <sys/socket.h>	// socket(), bind(), listen(), accept() and so on
#include <sys/select.h>	// select()
#include <stdlib.h>	// exit()
#include <stdio.h>	// printf(), perror()
#include <string.h>	// memset(), strncpy()
#include <unistd.h>	// unlink()
#include <signal.h>	// sigset()
#include <ctype.h>	// toupper()
#define MAX_CONNECTIONS 5
#define SOCK_PATH "sock_file\0"
#define BUF_SIZE 512

void unlink_sock_file_on_exit()
{
	if (!access(SOCK_PATH, F_OK))
	{
		unlink(SOCK_PATH);
	}
}

void sigcatch(int sig)
{
	if (sig == SIGINT && !access(SOCK_PATH, F_OK))
	{
		unlink(SOCK_PATH);
	}
	_exit(EXIT_FAILURE);
}

ssize_t read_lower_and_write_upper(int fd)
{
	char readbuf[BUF_SIZE];
	ssize_t read_sym_number = read(fd, readbuf, BUF_SIZE);
	if (read_sym_number!=0)
	{
		if (read_sym_number == -1)
		{
			perror("server read from socket");
			exit (EXIT_FAILURE);
		}
		int i = 0;
		for (i; i<read_sym_number; ++i)
		{
			readbuf[i] = toupper(readbuf[i]);
		}
		printf("%.*s", read_sym_number, readbuf);
	}
	return read_sym_number;
}

void set_handlers()
{
	if (atexit(unlink_sock_file_on_exit))
	{
		if (access(SOCK_PATH, F_OK))
		{
			unlink(SOCK_PATH);
		}
		fprintf(stderr, "set unlink on exit\n");
		exit(EXIT_FAILURE);
	}
	if (sigset(SIGINT, sigcatch) == SIG_ERR)
	{
		perror("set SIGINT handler");
		exit(EXIT_FAILURE);
	}
}


int create_server_socket()
{
	struct sockaddr_un sock_addr;

	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("server socket()");
		exit(EXIT_FAILURE);
	}
	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sun_family = AF_UNIX;
	strncpy(sock_addr.sun_path, SOCK_PATH, sizeof(sock_addr.sun_path)-1);

	if (bind(sock, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) == -1)
	{
		printf("%s\n", sock_addr.sun_path);
		perror("server bind()");
		exit(EXIT_FAILURE);
	}
	
	set_handlers();

	if (listen(sock, MAX_CONNECTIONS) == -1)
	{
		perror("server listen()");
		exit(EXIT_FAILURE);
	}
	return sock;
}


int main (int argc, char** argv)
{
	int sock = create_server_socket();

	fd_set ready_sock_descs;
	fd_set all_sock_descs;
	FD_ZERO(&ready_sock_descs);
	FD_ZERO(&all_sock_descs);

	int max_desc = sock;
	int clients_number = 0;

	printf("\nwaiting for connection\n");

	while (1)
	{
		if (clients_number < MAX_CONNECTIONS)
		{
			FD_SET(sock, &all_sock_descs);
		}
		else
		{
			FD_CLR(sock, &all_sock_descs);
		}
		int ready_num = 0;
		ready_sock_descs = all_sock_descs;
		if ((ready_num = select(max_desc+1, &ready_sock_descs, NULL, NULL, NULL))==-1)
		{
			perror("server select()");
			exit(EXIT_FAILURE);
		}
		if (FD_ISSET(sock, &ready_sock_descs))
		{
			int peer_sock = accept(sock, NULL, NULL );
			if (peer_sock == -1)
			{
				perror("server accept()");
				exit (EXIT_FAILURE);
			}
			FD_SET(peer_sock ,&all_sock_descs);
			if (peer_sock > max_desc)
			{
				max_desc = peer_sock;
			}
			++clients_number;
			--ready_num;
		
		}
		int i = sock+1;
		int eff_max_desc = max_desc;
		for (i; i<max_desc+1; i++)
		{
			if (FD_ISSET(i, &ready_sock_descs))
			{
				if (read_lower_and_write_upper(i)==0)
				{
					FD_CLR(i, &all_sock_descs);
					close(i);
					--clients_number;
					if (i == max_desc)
					{
						max_desc = eff_max_desc;	
					}
				}
				else
				{
					eff_max_desc = i;
				}
			}
		}
	}

}
