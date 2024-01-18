#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define SERVER_NAME "socketname"
#define BUF_LEN 30

int main() {
    int sockfd;
    char buf[BUF_LEN];
    struct sockaddr_un addr;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SERVER_NAME, sizeof(addr.sun_path) - 1);

    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket() caused an error ");
        exit(EXIT_FAILURE);
    } 

    unlink(SERVER_NAME);

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(sockfd);
        perror("bind() caused an error ");
        exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 6) == -1) {
        unlink(SERVER_NAME);
        close(sockfd);
        perror("listen() caused an error ");
        exit(EXIT_FAILURE);
    }

    int newsockfd;
    newsockfd = accept(sockfd, NULL, NULL);
    if (newsockfd == -1) {
        unlink(SERVER_NAME);
        close(sockfd);
        perror("accept() caused an error ");
        exit(EXIT_FAILURE);
    }

    ssize_t bytesRead;
    while ((bytesRead = read(newsockfd, buf, BUF_LEN)) > 0) {
        
        for (int i = 0; i < bytesRead; ++i) {
            buf[i] = toupper(buf[i]);
        }

        if (write(STDIN_FILENO, buf, bytesRead) == -1) {
            close(newsockfd);
            close(sockfd);
            unlink(SERVER_NAME);
            perror("write() caused an error ");
            exit(EXIT_FAILURE);
        }
    }
    if (bytesRead < 0) {
        close(newsockfd);
        close(sockfd);
        unlink(SERVER_NAME);
        perror("read() caused an error ");
        exit(EXIT_FAILURE);
    }

    unlink(SERVER_NAME);
    close(newsockfd);
    close(sockfd);
    exit(EXIT_FAILURE);


}