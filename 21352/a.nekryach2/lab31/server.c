#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <ctype.h>
#define PATH_TO_SOCKET "./server"
#define NUMBER_OF_CLIENTS 5 
#define BUFFER_SIZE 512

int serverSocket = 0;

void sigCatch(int sig)
{
    close(serverSocket);
    unlink(PATH_TO_SOCKET);
    exit(0);
}

void printUpper(char* buffer, ssize_t availableAmount)
{
    for(ssize_t counter =0; counter < availableAmount; counter++)
    {
        putchar(toupper(*buffer++));
    }
}
int main()
{
    char buffer[BUFFER_SIZE];
    serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(serverSocket == -1)
    {
        perror("Server socket cant be created");
        exit(1);
    }

    struct sockaddr_un serverAddress;
    memset(&serverAddress,0, sizeof(struct sockaddr_un));
    serverAddress.sun_family = AF_UNIX;
    strncpy(serverAddress.sun_path, PATH_TO_SOCKET, sizeof(serverAddress.sun_path) - 1);

 
    if(bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) == -1)
    {
        perror("Cant bind");
        exit(1);
    }
    sigset(SIGINT, sigCatch);
    sigset(SIGQUIT, sigCatch);


    if(listen(serverSocket, NUMBER_OF_CLIENTS) == -1)
    {
        unlink(PATH_TO_SOCKET);
        perror("Cant listen");
        exit(1);
    }

    struct pollfd serverFds[NUMBER_OF_CLIENTS + 1];
    memset(serverFds, 0, sizeof(serverFds));
    serverFds[0].fd = serverSocket;
    serverFds[0].events = POLLIN;
    for(nfds_t clientsSocketNumber = 1; clientsSocketNumber <= NUMBER_OF_CLIENTS; clientsSocketNumber++)
    {
        serverFds[clientsSocketNumber].fd = -1;
        serverFds[clientsSocketNumber].events = POLLIN;
    }
    nfds_t clientCounter = 0;
    nfds_t currentSockets = 1;
    int serverIsFull = 0;
    while(1)
    {
        if(poll((struct pollfd *)&serverFds[serverIsFull], currentSockets, -1) == -1)
        {
            perror("Cant using poll");
            unlink(PATH_TO_SOCKET);
            exit(1);
        }

        if((serverFds[0].revents & POLLIN) && (serverIsFull == 0)) 
        {
            int clientFd;
            if((clientFd = accept(serverSocket, NULL, NULL)) == -1)
            {
                perror("Cant accept");
                unlink(PATH_TO_SOCKET);
                exit(1);
            }
            clientCounter++;
            serverFds[clientCounter].fd = clientFd;
            currentSockets++;
            if(clientCounter == NUMBER_OF_CLIENTS)
            {
                serverIsFull = 1;
            }
        }

        for(nfds_t clientSocketNumber = 1; clientSocketNumber <= clientCounter; clientSocketNumber++)
        {
            if(serverFds[clientSocketNumber].revents & POLLIN)
            {
                size_t readSize = read(serverFds[clientSocketNumber].fd, buffer, BUFFER_SIZE);
                   
                if(readSize > 0)
                {
                    printUpper(buffer, readSize);
                }
                else
                {
                    close(serverFds[clientSocketNumber].fd);
                    serverFds[clientSocketNumber].fd = serverFds[clientCounter].fd;
                    serverFds[clientCounter].fd = -1;
                    clientCounter--;
                    currentSockets--;
                    serverIsFull = 0;
                }
            }

        }
         
    }

}
