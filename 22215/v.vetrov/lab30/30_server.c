#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define SOCKET_PATH "30_socket"
#define BUFFER_SIZE 100

void sig_handler(int signum)
{
  unlink(SOCKET_PATH);
  exit(0);
}

int main() {
  signal(SIGINT, sig_handler);

  int server_fd, client_fd;
  struct sockaddr_un server_addr;
  char buffer[BUFFER_SIZE];
  ssize_t num_read;

  server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("Error in creating socket.");
    return -1;
  }

  memset(&server_addr, 0, sizeof(struct sockaddr_un));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

  if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)) == -1) {
    perror("Error in bind.");
    return -1;
  }

  if (listen(server_fd, 5) == -1) {
    perror("Error in listen.");
    return -1;
  }

  printf("Server is waiting for connections...\n");

  client_fd = accept(server_fd, NULL, NULL);
  if (client_fd == -1) {
    perror("Error in accept.");
    return -1;
  }

  num_read = read(client_fd, buffer, BUFFER_SIZE);
  if (num_read > 0) {
    printf("Received: ");
    for (int i = 0; i < num_read; i++) {
      putchar(toupper(buffer[i]));
    }
    printf("\n");
  }

  close(client_fd);
  close(server_fd);
  unlink(SOCKET_PATH);

  return 0;
}