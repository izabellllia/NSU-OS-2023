#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>


int main(int argc, char **argv) {
    int descs[2];
    pid_t proc;
    char data[] = "aaAabbcA abbb";
    char *ptr = data;

    if (pipe(descs) == -1) {
        perror("Cannot create pipe");
        return 1;
    }

    proc = fork();

    if (proc == -1) {
        perror("Something went wrong in fork");
        close(descs[0]);
        close(descs[1]);
        return 1;
    }

    if (proc == 0) {
	close(descs[1]);
	char sym;
	while (read(descs[0], &sym, 1) != 0) {
		printf("%c", toupper(sym));
	}
	printf("\n");
	close(descs[0]);
    }
    else {
	close(descs[0]);
	ssize_t tmp = strlen(data);
	for (ssize_t n; tmp > 0; ){
		n = write(descs[1], ptr, tmp);
		if (n == -1){
			break;
		}
		ptr += n;
		tmp -= n;
	}
	if (tmp != 0){
		perror("error in writing");
		close(descs[1]);
		return 1;
	}
	close(descs[1]);
    }

    return 0;
}
