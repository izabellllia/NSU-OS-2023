#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

void intPerrorTrigger(int value, char* errorMessage) {
  if ((value) == -1) {
    perror(errorMessage);
    exit(-1);
  }
}

void handleSigInt(int signalValue) {
  exit(signalValue);
}

#define SERVER_SOCKET_NAME "socket.sock"
int clientSocketDescriptor;

void closeClientSocket() {
  printf("\nClient closed\n");
  intPerrorTrigger(close(clientSocketDescriptor), "Failed to close client socket");
}

int main(int argc, char** argv) {
  signal(SIGINT, handleSigInt);
  clientSocketDescriptor = socket(AF_UNIX, SOCK_STREAM, 0);
  intPerrorTrigger(clientSocketDescriptor, "Failed to create socket");
  atexit(closeClientSocket);
  struct sockaddr_un serverSocketAddr;
  memset(&serverSocketAddr, 0, sizeof(serverSocketAddr));
  serverSocketAddr.sun_family = AF_UNIX;
  strncpy(serverSocketAddr.sun_path, SERVER_SOCKET_NAME, sizeof(serverSocketAddr.sun_path)-1);
  
  intPerrorTrigger(
    connect(clientSocketDescriptor, (struct sockaddr*)&serverSocketAddr, sizeof(serverSocketAddr)),
    "Failed to connect with socket"
  );

  char buf[256] = "";
  int len;
  while ((len = read(0, buf, sizeof(buf))) != 0)
  {
    intPerrorTrigger(len, "Failed to read text for request");
    if (len == 1 && buf[0] == '\n')
      break;
    intPerrorTrigger(write(clientSocketDescriptor, buf, len), "Failed to send text by socket");
  }
  exit(0);
}