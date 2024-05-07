#include <grp.h>
#include <libgen.h>
#include <pwd.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

char get_type(const struct stat *file_stats)
{
    unsigned int st_mode = file_stats->st_mode;
    if (S_ISDIR(st_mode))
    {
        return 'd';
    }
    if (S_ISREG(st_mode))
    {
        return '-';
    }
    return '?';
}

void get_permissions(const struct stat *file_stats, char *output)
{
    unsigned int st_mode = file_stats->st_mode;

    output[0] = st_mode & S_IRUSR ? 'r' : '-';
    output[1] = st_mode & S_IWUSR ? 'w' : '-';
    output[2] = st_mode & S_IXUSR ? 'x' : '-';

    output[3] = st_mode & S_IRGRP ? 'r' : '-';
    output[4] = st_mode & S_IWGRP ? 'w' : '-';
    output[5] = st_mode & S_IXGRP ? 'x' : '-';

    output[6] = st_mode & S_IROTH ? 'r' : '-';
    output[7] = st_mode & S_IWOTH ? 'w' : '-';
    output[8] = st_mode & S_IXOTH ? 'x' : '-';
}


void print_user(const struct stat *file_stats)
{
    const struct passwd *passwd_ptr = getpwuid(file_stats->st_uid);
    if (passwd_ptr)
    {
        printf("%-*s ", 16, passwd_ptr->pw_name);
        return;
    }
	
    printf("%-*u ", 16, file_stats->st_uid);
}

void print_group(const struct stat *file_stats)
{
    const struct group *group_ptr = getgrgid(file_stats->st_gid);
    if (group_ptr)
    {
        printf("%-*s ", 16, group_ptr->gr_name);
        return;
    }

    printf("%-*u ", 16, file_stats->st_gid);
}

int main(int argc, char *argv[])
{
    for (int i = 1; i < argc; ++i)
    {
        struct stat file_stats;
        if (lstat(argv[i], &file_stats) == -1)
        {
            fprintf(stderr, "%s: %s\n", argv[i], strerror(errno));
            continue;
        }

        char file_type = get_type(&file_stats);
        char file_permissions[9];
        get_permissions(&file_stats, file_permissions);

        printf("%c", file_type);
        printf("%.9s ", file_permissions);
        printf("%-*u ", 16, file_stats.st_nlink);
        print_user(&file_stats);
        print_group(&file_stats);

        if (file_type == '-')
        {
            printf("%-*lld ", 16, (long long int)file_stats.st_size);
        }
        else
        {
            printf("%-*s ", 16, "");
        }

        printf("%.24s ", ctime(&(file_stats.st_mtime)));
	printf("%s\n", basename(argv[i]));
    }
    return 0;
}
