#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void try_open(char* filename){
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("error opening the file\n");
        return;
    }
    fclose(file);
}

int main(int argc, char *argv[]){

    printf("uid: %d; euid: %d\n", getuid(), geteuid());
    try_open(argv[1]);

    setuid(getuid());

    printf("uid: %d; euid: %d\n", getuid(), geteuid());
    try_open(argv[1]);
    return 0;
}