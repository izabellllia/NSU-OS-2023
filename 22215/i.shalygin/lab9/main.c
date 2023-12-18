#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
  pid_t pid;
  int status;
  switch(pid=fork()) {
  case -1:
          perror("fork"); 
          return -1; 
  case 0:
          execl("/bin/cat", "cat","some_textr",(char *)0);
          return 0;
  default:
          printf("PARENT: this is parent-process!\n");
	  wait(&status);
          printf("hello\n");
  }
  return 0;
}