FLAGS = -Wall -lsocket -lnsl -o

all: server client

server: lab30s.c
	gcc $(FLAGS) server lab30s.c

client: lab30.c
	gcc $(FLAGS) client lab30.c
	
clean:
	rm server client 