#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    srand(time(NULL));

    FILE *file_pointers[2];
    if (p2open("/bin/sort -n", file_pointers) == -1)
    {
        fprintf(stderr, "p2open failed\n");
        return -1;
    }

    for (int i = 0; i < 100; ++i)
    {
        fprintf(file_pointers[0], "%d\n", rand() % 100);
    }
    if (fclose(file_pointers[0]) != 0)
    {
        perror("fclose failed");
        return -1;
    }

    for (int i = 0; i < 10; ++i)
    {
        for (int j = 0; j < 10; ++j)
        {
            int current_number;
            fscanf(file_pointers[1], "%d", &current_number);
            printf("%2d ", current_number);
        }
        printf("\n");
    }

    pclose(file_pointers[1]);

    return 0;
}
