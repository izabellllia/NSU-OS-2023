// check necessary libraries


#include <sys/stat.h>
#include <stdbool.h>
#include <libgen.h>
#include <grp.h>
#include <pwd.h>

void printFtype(struct stat* s)
{
    if (S_ISDIR(s->st_mode))
    {
        printf("d");
    }
    else if (S_ISREG(s->st_mode))
    {
        printf("-");
    }
    else
    {
        printf("?");
    }
}

void printFpermissions(struct stat* s)
{
    printf("%c",(s->st_mode & S_IRUSR)?'r':'-');
    printf("%c",(s->st_mode & S_IWUSR)?'w':'-');
    printf("%c",(s->st_mode & S_IXUSR)?'x':'-');
    printf("%c",(s->st_mode & S_IRGRP)?'r':'-');
    printf("%c",(s->st_mode & S_IWGRP)?'w':'-');
    printf("%c",(s->st_mode & S_IXGRP)?'x':'-');
    printf("%c",(s->st_mode & S_IROTH)?'r':'-');
    printf("%c",(s->st_mode & S_IWOTH)?'w':'-');
    printf("%c",(s->st_mode & S_IXOTH)?'x':'-');
}

void print_time(time_t* change_time)
{
    time_t now;
    (void) time( &now );
    char format_string[7];
    char time_string[64];

    struct tm* structed_time = localtime(change_time);

    if (difftime(now, *change_time) > 15778800)
    {
        strftime(time_string, sizeof(time_string), "%b %Y", structed_time);
    }
    else
    {
        strftime(time_string, sizeof(time_string),  "%c", structed_time);
    }
    printf ("%s", time_string);
}

void ls(char* name)
{
    struct stat s = {0};
    if (stat(name, &s))
    {
        perror("Unable to get stat");
        return;
    }
    struct group* gr = getgrgid(s.st_gid);
    struct passwd* pw = getpwuid(s.st_uid);
    bool isFile = S_ISREG(s.st_mode);

    struct tm* change_time = localtime(&s.st_mtime);

    printFtype(&s);
    printFpermissions(&s);
    printf ("%5u", s.st_nlink);

    if (pw)
    {
        printf("%20s ", pw->pw_name);
    }
    else
    {
        printf("%20d", s.st_uid);
    }

    if (gr)
    {
        printf("%20s ", gr->gr_name);
    }
    else
    {
        printf("%20d", s.st_gid );
    }

    if (isFile)
    {
        printf("%20lld ",(long long int)s.st_size);
    }
    else
    {
        printf("%20s ","");
    }					   

    print_time(&s.st_mtime);
    printf(" %s\n", basename(name));
}

int main(int argc, char** args)
{
    if (argc < 2)
    {
        printf("Not enough args\n");
        return -1;
    }
    for (int i = 1; i < argc; ++i)
    {
        ls(args[i]);
    }
    return 0;
}
