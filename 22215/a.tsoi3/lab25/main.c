#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>

#define MSGSIZE 20

int main(int argc, char* argv[]) {
    int fd[2];

    if (pipe(fd) == -1) {
        perror(argv[0]);
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {   /* Child */
        close(fd[0]);
        char msgout[] = "I wAnT to EAt PiZZa";
        int written_bytes = 0;
        int curr_write;
        do {
            curr_write = write(fd[1], msgout, MSGSIZE);
            if (curr_write == -1) {
                perror("Error: Child write\n");
                return -1;
            }
            written_bytes += curr_write;
        } while (written_bytes < MSGSIZE);
        close(fd[1]);
    }
    else if (pid > 0) {  /* Parent */
        close(fd[1]);
        char msgin[MSGSIZE];
        if (read(fd[0], msgin, MSGSIZE) == -1) {
            perror("Error: Parent read\n");
        }
        close(fd[0]);

        for (int i = 0; i < MSGSIZE - 1; i++) {
            msgin[i] = toupper(msgin[i]);
        }
        printf("Hears: %s\n", msgin);
    }
    else {   /* Cannot fork */
        perror("Cannot fork\n");
        return 2;
    }

    return 0;
}
