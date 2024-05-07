#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
int main()
    {
        struct sockaddr_un addr;
        char buf[BUFSIZ];
        int fd,cl,rc;
        fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if ((fd) == -1) {
          perror("socket error");
          return -1;
        }
        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path,"socket",sizeof(addr.sun_path)-1); 
        if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
          perror("bind error");
          return -1;
        }
        if (listen(fd, 5) == -1) {
          perror("listen error");
	  unlink("socket");
          return -1;
        }
        while (1) {
          cl = accept(fd, NULL, NULL);
          if (cl == -1) {
            perror("accept error");
	    unlink("socket");
            return -1;
          }
	  printf("Connected\n");
          while (1){
            int x = read(cl, buf, BUFSIZ);
            if (x == 0){
                printf("\n");
                close(cl);
		unlink("socket");
                return 0;
                
            }
            else if (x == -1){
                unlink("socket");
		close(cl);
                perror("Couldn't read socket");
                return -1;
            }
            else{
              for (int i = 0;i<x;i++){
                  printf("%c",toupper(buf[i]));
              }
            }

          }
        }
        return 0;
    }