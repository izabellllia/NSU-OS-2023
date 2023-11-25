#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <wait.h>

int main() {
    pid_t child = fork();
    if (child == -1) {
        perror("Failed to fork");
        exit(1);
    } 
    if (child == 0) {
        if (execlp("cat", "cat", "bigboy.txt", NULL) == -1) {
            perror("Failed to execute cat");
            exit(1);
        }
        perror("Exec failed");
    } else {
        siginfo_t child_info;
        printf("%d\n", child);
        if (waitid(P_PID, child, &child_info, WEXITED) == -1) {
            perror("Failed to wait on child"); 
            exit(1);
        }
        printf("\nCat ended.\n"); 
    };

    return 0;
}
