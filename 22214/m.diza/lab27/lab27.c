#include <stdio.h>
#include <stdlib.h>

int main() {
    char command[] = "grep -c . file";
    char commandall[] = "wc -l file"; 
    int linesCount;
    int linesAllCount;

    FILE* fd = popen(command, "r");
    if (!fd) {
        perror("Pipe cretion failed");
        exit(1);
    }
    fscanf(fd, "%d", &linesCount);
    pclose(fd);

    FILE* fdall = popen(commandall, "r");
    if (!fdall) {
        perror("Pipe cretion failed");
        exit(1);
    }
    fscanf(fdall, "%d", &linesAllCount);
    pclose(fdall);
    
    
    printf("%d\n", linesAllCount - linesCount);
    return 0;
}