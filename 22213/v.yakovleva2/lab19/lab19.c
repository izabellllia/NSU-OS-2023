#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define CHUNK_SIZE 10

int matches(const char *name, const char *pattern) {
    while (*name && *pattern) {
        if (*pattern != '?' && *pattern != *name) {
            if (*pattern != '*') {
                return 0;
            }
            const char *p = pattern + 1;
            while (*p == '*') {
                p++;
            }
            if (*p == '\0') {
                return 1;
            }
            while (*name) {
                if (matches(name++, p)) {
                    return 1;
                }
            }
            return 0;
        }
        name++;
        pattern++;
    }
    while (*pattern == '*') {
            pattern++;
    }
    return *name == '\0' && *pattern == '\0';
}

int main() {
    size_t currentSize = CHUNK_SIZE;
    size_t usedSize = 0;
    char *pattern = (char *)malloc(currentSize * sizeof(char));

    if (pattern == NULL) {
        perror("Memory allocation error.\n");
        return 1;
    }

    printf("Enter pattern: ");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        if (usedSize + 1 >= currentSize) {
            currentSize += CHUNK_SIZE;
            char *temp = (char *)realloc(pattern, currentSize * sizeof(char));
            if (temp == NULL) {
                perror("Memory reallocation error.\n");
                free(pattern);
                return 1;
            }
            pattern = temp;
        }
        pattern[usedSize++] = (char)c;
    }
    pattern[usedSize] = '\0';

    DIR *dir;
    struct dirent *entry;
    int found = 0;

    if ((dir = opendir(".")) == NULL) {
        perror("Couldn't open current directory");
        free(pattern);
        return 1;
    }

    if (strchr(pattern, '/') != NULL) {
        printf("Using / is restricted\n");
        free(pattern);
        closedir(dir);
        return 0;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (matches(entry->d_name, pattern)) {
            printf("%s\n", entry->d_name);
            found = 1;
        }
    }

    if (errno) {
        printf("Error reading directory\n");
        free(pattern);
        closedir(dir);
        return 1;
    }

    if (!found) {
        printf("%s did not match any file in the current directory\n", pattern);
    }

    free(pattern);
    closedir(dir);
    return 0;
}
