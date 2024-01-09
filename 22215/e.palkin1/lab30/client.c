#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./socket"

int min (int a, int b)
{
	if (a <= b)
		return a;
	return b;
}

int main()
{
	struct sockaddr_un address;
	int client_fd;
	client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (client_fd < 0)
	{
		perror("socket error");
		return -1;
	}
	memset(&address, 0, sizeof(address));
	address.sun_family = AF_UNIX;
	strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path));
	if (connect(client_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		perror("error while connecting");
		return -1;
	}
	char *message = "Message for Server";
	
	int message_len = strlen(message);
	int offset = 0;
	int batch_size = 5;
	int bytes_write = 0;
	while (offset < message_len)
	{
		bytes_write = write(client_fd, message + offset, min(batch_size, message_len - offset));
		if (bytes_write < 0)
		{
			perror("Error while writing");
			return -1;
		}
		offset += bytes_write;
	}
	close(client_fd);
	return 0;
}
