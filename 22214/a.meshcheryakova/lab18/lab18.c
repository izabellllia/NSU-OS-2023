#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <libgen.h>

void file_info(char* filename) {
    struct stat file_stats;
    if (stat(filename, &file_stats) == -1) {
        perror("Error with stat");
        exit(1);
    }

    switch(file_stats.st_mode & S_IFMT) {
        case S_IFDIR:
            printf("d");
            break;
        case S_IFREG:
            printf("-");
            break;
        default:
            printf("?");
            break;
    }

    printf("%c%c%c%c%c%c%c%c%c ",
            file_stats.st_mode & S_IRUSR ? 'r' : '-',
            file_stats.st_mode & S_IWUSR ? 'w' : '-',
            file_stats.st_mode & S_IXUSR ? 'x' : '-',
            file_stats.st_mode & S_IRGRP ? 'r' : '-',
            file_stats.st_mode & S_IWGRP ? 'w' : '-',
            file_stats.st_mode & S_IXGRP ? 'x' : '-',
            file_stats.st_mode & S_IROTH ? 'r' : '-',
            file_stats.st_mode & S_IWOTH ? 'w' : '-',
            file_stats.st_mode & S_IXOTH ? 'x' : '-');

    printf("\t%lu", file_stats.st_nlink);

    struct passwd *pw;
    struct group *grp;

    pw = getpwuid(file_stats.st_uid);
    printf("\t%s", pw->pw_name);

    grp = getgrgid(file_stats.st_gid);
    printf("\t%s", grp->gr_name);

    if ((file_stats.st_mode & S_IFMT) == S_IFREG) {
        printf("\t%ld", file_stats.st_size);
    } else {
        printf("\t");
    }

    char lastChange[128];
    strftime(lastChange, 128, "%b %e %H:%M", localtime(&(file_stats.st_mtime)));
    printf("\t%s", lastChange);

    printf("\t%s\n", basename(filename));
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        file_info(".");
    } else {
        for(int i = 1; i < argc; i++) {
            file_info(argv[i]);
        }
    }
    return 0;
}
