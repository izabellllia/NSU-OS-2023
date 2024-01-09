#include <stdio.h>
#include <stdlib.h>
#include <glob.h>
#include <string.h>
#include <unistd.h>
int main()
{
    char **found;
    glob_t gstruct;
    int r;
    int size = sysconf(_SC_LINE_MAX);
    printf("%d\n",size);
    char* sample = (char*)calloc(size,sizeof(char));
    char* sample2 = (char*)calloc(2*size,sizeof(char));

    printf("Type your sample\n");
    scanf("%s",sample); 
    int curr = 0;
    for (int i =0;i<=strlen(sample);i++){
        if (sample[i] == '[' || sample[i] == ']')
            sample2[curr++] = '\\';
        sample2[curr++] = sample[i]; 
    }   
    r = glob(sample2, 0 , NULL, &gstruct);
    if( r!=0 )
    {
        if( r==GLOB_NOMATCH )
            printf("No files with such sample:%s\n",sample);
        else
            fprintf(stderr,"Some kinda glob error\n");
        return -1;
    }
    printf("Found:\n");
    found = gstruct.gl_pathv;
    while(*found)
    {
        printf("%s\n",*found);
        found++;
    }
    globfree(&gstruct);
    free(sample);
    free(sample2);
    return 0;
}