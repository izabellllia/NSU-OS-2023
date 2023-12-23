FLAGS = -Wall -lsocket -lnsl -o

all: server client

server: lab31s.c
	gcc $(FLAGS) server lab31s.c

client: lab31.c
	gcc $(FLAGS) client lab31.c
	
clean:
	rm server client 