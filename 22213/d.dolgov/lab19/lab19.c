#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <regex.h>
#include <errno.h>

#define MAX_PATTERN_LENGTH 100

struct dirent *file;
regex_t regex;

int letterCounter = 0;
int asteriskFound = 0;
char *pattern;

enum argCheckerStatuses {
    SUCCESS, TOO_MANY_ARGUMENTS, NO_ARGUMENTS
};

enum stringCheckerStatuses {
    STRING_CHECKER_SUCCESS, SLASH_FOUND
};

int argCheck(int argLength) {
    if (argLength == 2) {
        return SUCCESS;
    }

    if (argLength == 1) {
        return NO_ARGUMENTS;
    }

    return TOO_MANY_ARGUMENTS;
}

int checkString(char *searchString) {
    if (strstr(searchString, "/") != NULL) {
        return SLASH_FOUND;
    }

    return STRING_CHECKER_SUCCESS;
}

void parseLetter(const char *pattern, char *newPattern, int i, int *counter) {
    if (pattern[i] == '?') {
        newPattern[*counter] = '[';
        (*counter)++;
        newPattern[*counter] = 'A';
        (*counter)++;
        newPattern[*counter] = '-';
        (*counter)++;
        newPattern[*counter] = 'Z';
        (*counter)++;
        newPattern[*counter] = 'a';
        (*counter)++;
        newPattern[*counter] = '-';
        (*counter)++;
        newPattern[*counter] = 'z';
        (*counter)++;
        newPattern[*counter] = '0';
        (*counter)++;
        newPattern[*counter] = '-';
        (*counter)++;
        newPattern[*counter] = '9';
        (*counter)++;
        newPattern[*counter] = '_';
        (*counter)++;
        newPattern[*counter] = ']';
        (*counter)++;
        letterCounter++;
    } else if (pattern[i] == '*') {
        newPattern[*counter] = '.';
        (*counter)++;
        newPattern[*counter] = '*';
        (*counter)++;
        asteriskFound++;
    } else if (pattern[i] == '.') {
        newPattern[*counter] = '\\';
        (*counter)++;
        newPattern[*counter] = '.';
        (*counter)++;
        letterCounter++;
    } else {
        newPattern[*counter] = pattern[i];
        (*counter)++;
        letterCounter++;
    }
}

void buildRegexp(char *pattern_, int patternLength) {
    char *newPattern = malloc(sizeof(char) * MAX_PATTERN_LENGTH);

    if (newPattern == NULL) {
        perror("COULD NOT ALLOCATE MEMORY FOR REGEXP\n");
    }

    int counter = 0;

    for (int i = 0; i < patternLength; i++) {
        parseLetter(pattern_, newPattern, i, &counter);
    }

    int reti = regcomp(&regex, newPattern, 0);

    if (reti) {
        perror("COULD NOT COMPILE REGEX\n");
        exit(1);
    }

    free(newPattern);
}

void readFiles(DIR *folder, int *notFoundFlag) {

    while ((file = readdir(folder))) {
        errno = 0;

        char *fileName = malloc(sizeof(char) * (strlen(file->d_name) + 1));

        if (fileName == NULL) {
            perror("COULD NOT ALLOCATE MEMORY FOR FILENAME\n");
        }

        strcpy(fileName, file->d_name);
        if (regexec(&regex, fileName, 0, NULL, 0) == 0) {
            if (asteriskFound == 0 && letterCounter == strlen(file->d_name)) {
                printf("%s ", file->d_name);
                (*notFoundFlag)++;
                continue;
            } else if (asteriskFound > 0) {
                int wrongFilename = 0;
                int patternCounter = (int) strlen(pattern);
                for (int i = (int) strlen(file->d_name); i >= 0; i--) {
                    if (pattern[patternCounter] == '?' || pattern[patternCounter] == '*') {
                        *notFoundFlag = 0;
                        break;
                    }

                    if (pattern[patternCounter] == file->d_name[i]) {
                        patternCounter--;
                    } else {
                        wrongFilename = 1;
                        break;
                    }
                }
                if (wrongFilename != 1) {
                    printf("%s ", file->d_name);
                    (*notFoundFlag)++;
                }
                continue;
            }
        } else if (asteriskFound == 0 && strlen(fileName) > letterCounter) {
            (*notFoundFlag)++;
        }

        free(fileName);
    }

    if (errno != 0) {
        perror("ERROR READING DIRECTORY\n");
    }

    closedir(folder);
}

void findFiles(char *pattern_, int patternLength) {
    DIR *folder = opendir(".");
    int notFoundFlag = 0;

    if (folder == NULL) {
        perror("UNABLE TO READ THE DIRECTORY\n");
        exit(1);
    }

    buildRegexp(pattern_, patternLength);
    readFiles(folder, &notFoundFlag);

    if (notFoundFlag == 0) {
        printf("%s", pattern_);
    }

    printf("\n");
}

void propagate(char *rawPattern) {
    int patternLength = (int) strlen(rawPattern);
    char *pattern = malloc(sizeof(char) * patternLength);

    if (pattern == NULL) {
        perror("COULD NOT ALLOCATE MEMORY FOR PATTERN\n");
    }

    strcpy(pattern, rawPattern);

    if (checkString(pattern) == SLASH_FOUND) {
        perror("NO SLASHES ALLOWED IN THE SEARCH PATTERN\n");
    } else {
        findFiles(pattern, patternLength);
    }

    free(pattern);
}

char *patternInput() {
    printf("Please enter your file pattern\n");
    char *pattern_ = malloc(sizeof(char) * MAX_PATTERN_LENGTH);

    if (pattern_ == NULL) {
        perror("COULD NOT ALLOCATE MEMORY FOR PATTERN\n");
    }

    scanf("%s", pattern_);

    return pattern_;
}

int main(int argc, char *argv[]) {
    pattern = patternInput();

    propagate(pattern);

    free(pattern);
    exit(0);
}
