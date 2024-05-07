#include <sys/socket.h>	// socket(), connect()
#include <stdlib.h>	// exit()
#include <fcntl.h>	// open()
#include <unistd.h>	// write(), read(), close()
#include <stdio.h>	// perror()
#include <string.h>	// memset(), strncpy()

#define SOCK_PATH "sock_file\0"
#define BUF_SIZE 512

void write_in_socket(int file_handle, int sock)
{
	char string[BUF_SIZE];
	int bytes_read;

	while ((bytes_read=read(file_handle, string, BUF_SIZE)) != 0)
	{
		if (bytes_read == -1)
		{
			perror("client read from file");
			exit(EXIT_FAILURE);
		}
		write(sock, string, bytes_read);
	}
}


int main(int argc, char** argv)
{
	if (argc!=2)
	{
		fprintf(stderr, "Wrong number of arguments. Allowed number: 1\n");
		exit(EXIT_FAILURE);
	}
	int file_handle = open(argv[1], O_RDONLY);
	if (file_handle == -1)
	{
		perror("parent hasn't read the file");
		exit(EXIT_FAILURE);
	}


	struct sockaddr_un sock_addr;
	int sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("client socket()");
		exit(EXIT_FAILURE);
	}
	memset(&sock_addr, 0, sizeof(sock_addr));
	sock_addr.sun_family = AF_UNIX;
	strncpy(sock_addr.sun_path, SOCK_PATH, sizeof(sock_addr.sun_path)-1);
	if (connect(sock, (struct sockaddr*) &sock_addr, sizeof(sock_addr)))
	{
		perror("client connect()");
		close(sock);
		exit(EXIT_FAILURE);
	}
	
	write_in_socket(file_handle, sock);
	if (close(sock))
	{
		perror("client close socket");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
