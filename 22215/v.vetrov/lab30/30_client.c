#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "30_socket"

int main() {
  int socket_fd;
  struct sockaddr_un server_addr;
  char message[] = "Hello, Server!";

  socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (socket_fd == -1) {
    perror("Error in creating socket.");
    return -1;
  }

  memset(&server_addr, 0, sizeof(struct sockaddr_un));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

  if (connect(socket_fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_un)) == -1) {
    perror("Error in connection.");
    return -1;
  }

  if (write(socket_fd, message, strlen(message)) != strlen(message)) {
    perror("Error in writing.");
    return -1;
  }

  printf("Message sent to server.");

  close(socket_fd);

  return 0;
}
