#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>
#include <ctype.h>
#include <fcntl.h>
#define SIZE 512
#define PATH_TO_SOCKET "./server"
int clientSocket;
int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "No file\n");
        exit(0);
    }

    int f_d;

    if((f_d = open(argv[1], O_RDONLY)) == -1)
    {
        perror("Cant open file");
        exit(1);
    }
    
    clientSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(clientSocket == -1)
    {
         perror("Cant create socket");
	 exit(1);
    }

    struct sockaddr_un clientAddress;
    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddress.sun_family = AF_UNIX;
    strncpy(clientAddress.sun_path, PATH_TO_SOCKET, sizeof(clientAddress.sun_path) - 1);
    
    if(connect(clientSocket, (struct sockaddr*) &clientAddress, sizeof(clientAddress)) == -1) 
    { 
         perror("Cant connect");
	 exit(1);
    }
    char outputBuf[SIZE];
    ssize_t readFromFileSize;
    while((readFromFileSize = read(f_d, outputBuf, SIZE)) != 0)
    {
         write(clientSocket, outputBuf, readFromFileSize);
    }

    exit(0);
}
