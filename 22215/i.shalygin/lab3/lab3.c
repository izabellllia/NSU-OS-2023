#include <stdio.h>
#include <unistd.h>
void main(int argc, char* argv[])
{
    printf("User real id is %d\n", getuid());
    printf("User effective is %d\n", geteuid());
    FILE* file= fopen("file","r");
    if (file == NULL)
    {
        perror("Couldn't open file");
    }
    else{
        fclose(file);
    }
    if (setuid(getuid()) == -1)
    {
        perror("Couldn't change setuid");
    }
    
    printf("User real id is %d\n", getuid());
    printf("User effective is %d\n", geteuid());
    file= fopen("file"  ,"r");
    if (file == NULL)
    {
        perror("Couldn't open file");
    }
    else{
        fclose(file);
    }

}
