#include <stdio.h>
#include <unistd.h>
extern char **environ;

int execpve(const char *file, char * argv[], char * envp[]) {
    char ** environ_ = environ;
    environ = envp;
    execvp(file, argv);
    environ = environ_;
    return -1;
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        fprintf(stderr, "No executed file\n");
        return 0;
    }
    char *nenv[] = {"NAME=value","nextname=nextvalue","HOME=/xyz","PATH=.",(char *) 0 };
    execpve(argv[1], argv + 1, nenv);
    perror("This line should never get printed");
    return 0;
}
