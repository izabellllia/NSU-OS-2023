#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

extern char** environ;

int execvpe(const char* filename, char** argv, char** envp)
{
    char** tmp_environ = environ;
    environ = envp;
    execvp(filename, argv);
    environ = tmp_environ;
    return -1;
}

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s cmd\n", argv[0]);
        exit(1);
    }
    char *new_environ[] = {"LOGNAME=my_name", "HOME=/homerep", "PATH=/bin/:.", NULL};
    if (execvpe(argv[1], &argv[1], new_environ) == -1) {
        perror("execvpe");
    }
    return 0;
}
