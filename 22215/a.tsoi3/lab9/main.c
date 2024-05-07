#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    pid_t pid = fork();

    if (pid < 0) {  // Error case
        perror("Couldn't make a subprocess");
    }
    else if (pid == 0) {  // Code for subprocess
        execl("/bin/cat", "cat", "big_file",(char *)0);
    }
    else { // Code for current process
        if (wait(0) == -1) {
            perror("Couldn't wait completion of a subprocess");
        }
        printf("GOD, please, help me survive this course\n");
    }

    return 0;
}
