CC = gcc
CFLAGS = -c -Wall
LFLAGS = -lsocket
TARGETS = server client
SERVER_DEPENDENCIES = server.o utility.o
CLIENT_DEPENDENCIES = client.o utility.o

all: $(TARGETS)

server: $(SERVER_DEPENDENCIES)
	$(CC) $(LFLAGS) $(SERVER_DEPENDENCIES) -o server

client: $(CLIENT_DEPENDENCIES)
	$(CC) $(LFLAGS) $(CLIENT_DEPENDENCIES) -o client

server.o: server.c utility.h
	$(CC) $(CFLAGS) server.c

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) utility.c

client.o: client.c utility.h
	$(CC) $(CFLAGS) client.c

clean:
	rm -rf *o
	rm $(TARGETS)
