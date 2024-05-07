#include <stdio.h>
#include <unistd.h>
#include <errno.h>

extern int errno;

static void open_close_file(const char* const  path_to_file)
{
    FILE* file = fopen(path_to_file, "r");

    if (!file)
    {
        perror("Failure while opening file\n\n");
        return;
    }

    printf("File was opened successfully!\n\n");
    fclose(file);
}

int main(const int argc, const char** argv)
{
    const char filename[] = {"/home/students/20300/a.oborin/NSU-OS-2023/21345/a.oborin/lab3/data"};

    printf("real user id: %d\neffective user id: %d\n", getuid(), geteuid());
    open_close_file(filename);

    if (seteuid(getuid()) != 0)
    {
	    perror("Something went wrong. Can not change uid.");
	    return errno;
    }

    printf("After seteuid():\nreal user id: %d\neffective user id: %d\n", getuid(), geteuid());
    open_close_file(filename);

    return 0;
}
