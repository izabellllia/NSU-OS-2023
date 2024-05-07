  #include <sys/types.h>
  #include <unistd.h>
  #include <ctype.h>
  #include <stdlib.h>
  #include <sys/wait.h>
  #include <stdio.h>
  #include <string.h>
  int main()
  {

       char sended_message[BUFSIZ];
       char recieved_message[BUFSIZ];
       int fd[2]; 
       if (pipe(fd) == -1) {
            perror("Couldn't open pipe"); 
            return -1;
       }
       pid_t pid = fork();
       int size = 0;

       if (pid > 0) {       /* parent */
            close(fd[0]);
            printf("Write your text:\n");
	     while (1){
               if (fgets(sended_message,BUFSIZ,stdin) == NULL){
                    perror("Couldn't read stdin");
                    break;
               }
               
               size = strlen(sended_message);
               if(write(fd[1], sended_message, size) == -1){
                    perror("Couldn't write in pipe");
                    break;
               } 
               if (sended_message[size-1] == '\n'){
                    break;
               }
          }     
	    close(fd[1]);

       }
       else if(pid == 0)  {  
            close(fd[1]); 
            while (1){
                  int x = read(fd[0], recieved_message, BUFSIZ);
                  if (x == 0){
                    break;
                  }
		  else if (x == -1){
		  	perror("Couldn't read pipe");
               close(fd[0]);  
			return -1;
		  }
		  else{
			for (int i = 0;i<x;i++){
			    printf("%c",toupper(recieved_message[i]));
			}	  	
		  }   
            }
            close(fd[0]);   
            return 0;
       }
       else {     
             perror("Couldn't fork");
             return -1;
       }

       return 0;
  }