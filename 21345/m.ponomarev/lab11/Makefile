CC = cc

SRCS = my_execvpe.c print_env.c

EXECS = $(SRCS:.c=.out)

all: $(EXECS)

my_execvpe.out: my_execvpe.c
	$(CC) my_execvpe.c -o my_execvpe.out

print_env.out: print_env.c
	$(CC) print_env.c -o print_env.out

clean:
	rm -f $(EXECS)
