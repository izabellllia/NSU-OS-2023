#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
    return 1;
  }

  pid_t pid = fork();

  if (pid < 0) {
    perror("fork failed");
    return 2;
  }

  if (pid == 0) {
    // Дочерний процесс
    execvp(argv[1], &argv[1]);
    perror("execvp failed");
    return 3;
  } else {
    // Родительский процесс
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid failed");
      return 4;
    }

    if (WIFEXITED(status)) {
      printf("Child exited with status %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("Child terminated by signal %d\n", WTERMSIG(status));
    }

    return 0;
  }
}
