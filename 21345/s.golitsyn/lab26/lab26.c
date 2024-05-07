#include <stdio.h>

int main()
{
    char text[] = "Hello World12313!\n";

    FILE *pipe = popen("./to_upper", "w");
    if (pipe == NULL)
    {
        perror("popen error");
        return -1;
    }

    if (fwrite(text, 1, sizeof(text), pipe) != sizeof(text))
    {
        perror("fwrite error");
        return -1;
    }

    pclose(pipe);

    return 0;
}
