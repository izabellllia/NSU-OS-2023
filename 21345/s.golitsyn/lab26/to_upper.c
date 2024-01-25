#include <unistd.h>
#include <ctype.h>
#include <stdio.h>

#define BUFSIZE 1024

int main()
{
    char buf[BUFSIZE];
    ssize_t sym_read = 0;

    while ((sym_read = read(STDIN_FILENO, buf, BUFSIZE)) > 0)
    {
        for (int i = 0; i < sym_read; ++i)
        {
            buf[i] = toupper(buf[i]);
        }

        if (fwrite(buf, 1, sym_read, stdout) != sym_read)
        {
            perror("fwrite error");
            return -1;
        }
    }

    if (sym_read == -1)
    {
        perror("read error");
        return -1;
    }

    return 0;
}
