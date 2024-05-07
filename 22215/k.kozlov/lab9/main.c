#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/wait.h"


int main(int argc, char** argv, char** envp) {
  pid_t child = fork();
  if (child == -1) {
    perror("Failed to create child");
    exit(-1);
  }
  int status;
  if (child == 0) {
    char* args[] = {"cat", "./test.txt", NULL};
    if (execve("/bin/cat", args, envp) == -1) {
      perror("Failed to execute /bin/cat");
      exit(-1);
    }
  }
  if (waitpid(child, &status, 0) == -1) {
    perror("Error got while waiting for the child");
    exit(-1);
  }
  if (WIFEXITED(status))
    printf("\nI'm a parent!\n");    
  return 0;
}