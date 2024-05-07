#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <file_name>\n", argv[0]);
    return 1;
  }
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork failed");
    return 2;
  }
  if (pid == 0) {
    execl("/bin/cat", "cat", argv[1], NULL);
    perror("execl failed");
    return 3;
  }
  if (wait(0) == -1) {
    perror("wait failed");
    return 4;
  }
  printf("The file content was displayed by the child process.");
  return 0;
}
