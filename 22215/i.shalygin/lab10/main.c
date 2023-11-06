#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[])
{
  pid_t pid;
  int status;
  if (argc < 2){
      printf("No command passed\n");
      return -1;
  }
  
  switch(pid=fork()) {
  case -1:
          perror("fork doesn't work"); 
          return -1; 
  case 0:
          execvp(argv[1],argv+1);
          return 0;
  default:
	  wait(&status);
          printf("Code %d\n",status);
  }
  return 0;
}