#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void outputID_workWithFile(char *name) {
    printf("real id: %d, effective id: %d\n", getuid(), geteuid());

    FILE *file = fopen(name, "r");

    if (file == NULL) {
        perror("error opening file");
        return;
    }
    if (fclose(file) == EOF) {
        perror("error closing file");
	return;
    }
}

int main(int argc, char *argv[]) {
    outputID_workWithFile(argv[1]);

    int setNewUID = seteuid(getuid());
    if (setNewUID == -1) {
        perror("failed to set new user ID\n");
	return 1;
    }

    outputID_workWithFile(argv[1]);
    return 0;
}
