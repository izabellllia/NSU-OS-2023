#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <limits.h>
#include <stdint.h>

extern int errno;
extern char** environ;
extern char* optarg;
extern int optopt, opterr;

static void help()
{
	fprintf(stderr, 
		"-i - print real and effective user and group ID`s\n"
		"-s - make current process leader of the group\n"
		"-p - print process, parent-process and group ID`s\n"
		"-u - print current max file size\n"
		"-Unew_ulimit - set max file size to 'new_ulimit'\n"
		"-c - print core file size\n"
		"-Csize - set core file size to 'size'\n"
		"-d - print current working directory\n"
		"-v - print env variables\n"
		"-Vname=value - change env variable or create the new one if it does not exist yet\n\n");
}

static rlim_t optarg_to_rlim_t(const char* optarg)
{
	if (sizeof(long long) == sizeof(rlim_t))
	{
		return (rlim_t)strtoll(optarg, NULL, 10);
	}
	else
	{
		return (rlim_t)strtol(optarg, NULL, 10);
	}
}

static void handle_h()
{
	help();
}

static void handle_i()
{
	printf("Real user ID: %d\n", getuid());
	printf("Effective user ID: %d\n", geteuid());
	printf("Real users group ID: %d\n", getgid());
	printf("Effective users group ID: %d\n\n", getegid());
}

static void handle_s()
{
	if (setpgid(0, 0) == -1)
	{
		perror("Unable to set process as a leader of the group");
	}
}

static void handle_p()
{
	printf("Proccess ID: %d\n", getpid());
	printf("Parent process ID: %d\n", getppid());
	printf("Processes group ID: %d\n\n", getpgid(0));
}

static void handle_u()
{
	struct rlimit u_limit;

	if (getrlimit(RLIMIT_FSIZE, &u_limit) == -1)
	{
		perror("Unable to get max file size");
		return;
	}

	if (u_limit.rlim_cur == RLIM_INFINITY)
	{
		printf("Max file size soft: unlimited\n");
	}
	else
	{
		printf("Max file size soft: %jd\n", (intmax_t)u_limit.rlim_cur);
	}

	if (u_limit.rlim_max == RLIM_INFINITY)
	{
		printf("Max file size hard: unlimited\n\n");
	}
	else
	{
		printf("Max file size hard: %jd\n\n", (intmax_t)u_limit.rlim_max);
	}
}

static void handle_U()
{
	struct rlimit new_u_limit;
	if (getrlimit(RLIMIT_FSIZE, &new_u_limit) == -1)
	{
		perror("Unable to get current max file size");
		return;
	}

	errno = 0;
	const rlim_t new_soft_limit = optarg_to_rlim_t(optarg);
	if (errno != 0)
	{
		perror("Can not convert -U argument");
		return;
	}
	new_u_limit.rlim_cur = new_soft_limit;

	if (setrlimit(RLIMIT_FSIZE, &new_u_limit) == -1)
	{
		perror("Unable to change max file size");
	}
}

static void handle_c()
{
	struct rlimit core_limit;
	if (getrlimit(RLIMIT_CORE, &core_limit) == -1)
	{
		perror("Unable to get core limit");
	}

	if (core_limit.rlim_cur == RLIM_INFINITY)
	{
		printf("Core limit soft: unlimited\n");
	}
	else
	{
		printf("Core limit soft: %jd\n", (intmax_t)core_limit.rlim_cur);
	}

	if (core_limit.rlim_max == RLIM_INFINITY)
	{
		printf("Core limit hard: unlimited\n\n");
	}
	else
	{
		printf("Core limit hard: %jd\n\n", (intmax_t)core_limit.rlim_max);
	}
}

static void handle_C()
{
	struct rlimit new_core_limit;
	if (getrlimit(RLIMIT_CORE, &new_core_limit) == -1)
	{
		perror("Unable to get current core limit");
		return;
	}

	errno = 0;
	const rlim_t new_soft_limit = optarg_to_rlim_t(optarg);
	if (errno != 0)
	{
		perror("Can not convert -C argument");
		return;
	}
	new_core_limit.rlim_cur = new_soft_limit;

	if (setrlimit(RLIMIT_CORE, &new_core_limit) == -1)
	{
		perror("Unable to change core limit");
	}
}

static void handle_d()
{
	const long max_path_length = pathconf("/", _PC_PATH_MAX);
	char* cwd = getcwd(NULL, max_path_length + 1);
	if (cwd == NULL)
	{
		perror("getcwd");
		return;
	}

	printf("Current working directory: %s\n\n", cwd);
	free(cwd);
}

static void handle_v()
{
	for (char** env = environ; *env; ++env)
	{
		printf("%s\n", *env);
	}
	printf("\n");
}

static void handle_V()
{
	if (putenv(optarg) == -1)
	{
		perror("putenv");
	}
}

static void handle_arg(const int argument)
{
	switch (argument)
	{
	case 'h':
		handle_h();
		break;

	case 'i':
		handle_i();
		break;

	case 's':
		handle_s();
		break;

	case 'p':
		handle_p();
		break;

	case 'u':
		handle_u();
		break;

	case 'U':
		handle_U();
		break;

	case 'c':
		handle_c();
		break;

	case 'C':
		handle_C();
		break;

	case 'd':
		handle_d();
		break;

	case 'v':
		handle_v();
		break;

	case 'V':
		handle_V();
		break;

	case '?':
		printf("Invalid option: %c\n\n", (char)optopt);
	}
}

int main(int argc, char** argv)
{
	const char options[] = "hispuU:cC:dvV:";

	if (argc < 2)
	{
		help();
		return 0;
	}

	int argument;
	opterr = 0;

	while ((argument = getopt(argc, argv, options)) != -1)
	{
		handle_arg(argument);
	}

	return 0;
}
