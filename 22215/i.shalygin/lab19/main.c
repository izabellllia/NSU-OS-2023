#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fnmatch.h>
#include <stdlib.h>


int main() {
    DIR *dir;
    struct dirent *entry;
    int size = sysconf(_SC_LINE_MAX);
    printf("%d\n",size);
    char* sample = (char*)calloc(size,sizeof(char));
    char* sample2 = (char*)calloc(2*size,sizeof(char));
    printf("Type your sample\n");
    scanf("%s",sample);

    int curr = 0;
    int sample_len = strlen(sample);
    for (int i = 0;i<sample_len;i++){
        if (sample[i] == '[' || sample[i] == ']'){
	    sample2[curr] = '\\';
	    curr++;
	}
        sample2[curr] = sample[i];
	curr++; 
    }
    dir = opendir("./");
    if (!dir) {
        perror("Dir couldn't open");
        return -1;
    }
    
    int empty = 0;
    entry = readdir(dir);
    while (entry != NULL) {
        if(fnmatch(sample2, entry->d_name,0) == 0){
            printf("%s\n",entry->d_name);
            empty = 0;
        }
	entry = readdir(dir);
    }
    if (empty)
        printf("No files with such sample:%s\n",sample);
    closedir(dir);
    free(sample);
    free(sample2);
    return 0;
};