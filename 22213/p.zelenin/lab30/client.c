#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

#define SERVER_NAME "socketname"
#define BUF_LEN 30

int sockfd = -1;

void sigint_handler();
void sigpipe_handler();

int main() {
    struct sockaddr_un addr;
    char string[BUF_LEN];

    signal(SIGPIPE, sigpipe_handler);
    signal(SIGINT, sigint_handler);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SERVER_NAME, sizeof(addr.sun_path) - 1);


    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket() caused an error ");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(sockfd);
        perror("connect() caused an error ");
        exit(EXIT_FAILURE);
    }

    ssize_t bytesRead;

    while ((bytesRead = read(STDIN_FILENO, string, BUF_LEN)) > 0) {
        if (write(sockfd, string, bytesRead) == -1) {
            close(sockfd);
            perror("writing a string to socket caused an error ");
            exit(EXIT_FAILURE);
        }
    }
    if (bytesRead < 0) {
        close(sockfd);
        perror("read() caused an error ");
        exit(EXIT_FAILURE);
    }
    
}

void sigpipe_handler() {
    if (sockfd != -1) {
        close(sockfd);
        write(STDERR_FILENO, "SIGPIPE: writing into socket caused an error ", 37);
    }

    _exit(EXIT_FAILURE);
}

void sigint_handler() {
    if (sockfd != -1) {
        close(sockfd); 
    }
    _exit(EXIT_FAILURE);
}