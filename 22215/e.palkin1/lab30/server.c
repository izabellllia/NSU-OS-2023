#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#define SOCKET_PATH "./socket"

int main()
{
	struct sockaddr_un addr;
	int server_fd;
	int reader;
	int client;
	char buffer[100];
	if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
	{
		perror("socket error");
		return -1;
	}

	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path));

	if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
	{
		perror("bind error");
		return -1;
	}

	if (listen(server_fd, 5) < 0)
	{
		perror("listen error");
		return -1;
	}

	if ((client = accept(server_fd, NULL, NULL)) < 0)
	{
		perror("error while accepting");
		return -1;
	}
	while ((reader = read(client, buffer, sizeof(buffer))) > 0)
	{
		for (int i = 0;i < reader; i++)
		{
			printf("%c", toupper(buffer[i]));
		}
	}
	if (reader == -1)
	{
		perror("error while reading");
		return -1;
	}
	else if (reader == 0)
	{
		close(client);
		close(server_fd);
		return 0;
	}
	return 0;
}
