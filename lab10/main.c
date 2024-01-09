#include "unistd.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/wait.h"

pid_t forkWrapper() {
  pid_t child = fork();
  if (child == -1) {
    perror("Failed to create child");
    exit(-1);
  }
  return child;
}

void execWrapper(int argc, char** argv) {
  if (argc < 2) {
      printf("You haven't specified command for execution\n");
      exit(-1);
    }
    char* command = argv[1];
    char** childArgs = argv + 1;
    int processNameOffset = 0, i = 0;
    while (childArgs[0][i] != 0) {
      if (childArgs[0][i] == '/')
        processNameOffset = i;
      i++;
    }
    childArgs[0] += processNameOffset;
    printf("Executing %s...\n", command);
    if (execvp(command, childArgs) == -1) {
      perror("Failed to execute");
      exit(-1);
    }
}

int main(int argc, char** argv, char** envp) {
  pid_t child = forkWrapper();
  int status;
  if (child == 0) {
    execWrapper(argc, argv);
  }
  if (waitpid(child, &status, 0) == -1) {
    perror("Error got while waiting for the child");
    exit(-1);
  }
  if (WIFEXITED(status))
    printf("\nChild finished with exit status %d\n", WEXITSTATUS(status));
  return 0;
}






