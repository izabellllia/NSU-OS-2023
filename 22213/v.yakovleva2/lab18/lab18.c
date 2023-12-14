#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <libgen.h>

int main(int argc, char **file_paths) {
    // Iterate through command-line arguments (file paths)
    for(int file_index = 1; file_index < argc; ++file_index) {
        struct stat file_stats;

        // Get file information using stat() function
        if (stat(file_paths[file_index], &file_stats) == -1) {
            perror(file_paths[file_index]); // Print error if stat() fails
            exit(1);
        }

        // Determine file type and print respective symbol
        switch(file_stats.st_mode & S_IFMT) {
            case S_IFDIR:
                printf("d"); // Directory
                break;
            case S_IFREG:
                printf("-"); // Regular file
                break;
            default:
                printf("?"); // Other file type
                break;
        }

        // Print file permissions using bitwise operations
        printf("%c%c%c%c%c%c%c%c%c",
                file_stats.st_mode & S_IRUSR ? 'r' : '-',
                file_stats.st_mode & S_IWUSR ? 'w' : '-',
                file_stats.st_mode & S_IXUSR ? 'x' : '-',
                file_stats.st_mode & S_IRGRP ? 'r' : '-',
                file_stats.st_mode & S_IWGRP ? 'w' : '-',
                file_stats.st_mode & S_IXGRP ? 'x' : '-',
                file_stats.st_mode & S_IROTH ? 'r' : '-',
                file_stats.st_mode & S_IWOTH ? 'w' : '-',
                file_stats.st_mode & S_IXOTH ? 'x' : '-');

        // Print number of hard links, owner name, group name
        printf(" %lu", file_stats.st_nlink);
        printf(" %s", getpwuid(file_stats.st_uid)->pw_name);
        printf(" %s", getgrgid(file_stats.st_gid)->gr_name);

        // If it's a regular file, print its size
        if ((file_stats.st_mode & S_IFMT) == S_IFREG) {
            printf(" %ld", file_stats.st_size);
        }

        printf(" %.24s ", ctime(&(file_stats.st_mtime))); // Print formatted time
        printf(" %s\n", basename(file_paths[file_index])); // Print filename
    }

    return 0;
}
