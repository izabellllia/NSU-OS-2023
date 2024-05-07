#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#define CHUNK_SIZE 10

int patternFound = 0;

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

int isDirectory(const char *path) {
    struct stat path_stat;
    if (stat(path, &path_stat) != 0) {
        return 0;
    }
    return S_ISDIR(path_stat.st_mode);
}

void searchFiles(const char *basePath, const char *pattern) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(basePath)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            char *fullpath = NULL;
            size_t pathLen = strlen(basePath) + strlen(entry->d_name) + 2; // 2 for '/' and null terminator
            fullpath = (char *)malloc(pathLen * sizeof(char));
            if (fullpath == NULL) {
                perror("Memory allocation failed\n");
                closedir(dir);
                return;
            }
            snprintf(fullpath, pathLen, "%s/%s", basePath, entry->d_name);

            if (strncmp(fullpath, "./", 2) == 0) {
                memmove(fullpath, fullpath + 2, strlen(fullpath));
            }

            if (matches(fullpath, pattern)) {
                printf("%s\n", fullpath);
                patternFound = 1;
            }

            if (isDirectory(fullpath) && strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                searchFiles(fullpath, pattern);
            }

            free(fullpath);
        }
        if (errno) {
            printf("Error reading directory\n");
            closedir(dir);
            return;
        }
        closedir(dir);
    } else {
        perror("opendir failed.\n");
        return;
    }
}


int main() {
    char chunk[CHUNK_SIZE];
    char *pattern = NULL;
    size_t patternSize = 0;

    printf("Enter pattern: ");

    while (1) {
        if (fgets(chunk, sizeof(chunk), stdin) == NULL) {
            perror("Error reading pattern\n");
            return 1;
        }

        size_t chunkLen = strlen(chunk);

        if (chunk[chunkLen - 1] == '\n') {
            chunk[chunkLen - 1] = '\0';
            chunkLen--;
        }

        char *temp = realloc(pattern, patternSize + chunkLen + 1);
        if (temp == NULL) {
            perror("Memory allocation failed\n");
            free(pattern);
            return 1;
        }
        pattern = temp;

        memcpy(pattern + patternSize, chunk, chunkLen);
        patternSize += chunkLen;

        if (chunkLen < CHUNK_SIZE - 1) {
            pattern[patternSize] = '\0'; // Null-terminate the pattern
            break;
        }
    }

    searchFiles(".", pattern);

    if (!patternFound) {
        printf("No files matching '%s' pattern found\n", pattern);
    }

    free(pattern);

    return 0;
}
