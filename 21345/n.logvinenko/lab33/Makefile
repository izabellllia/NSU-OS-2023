CC = gcc
CFLAGS = -c -Wall
LFLAGS = -lsocket
TARGETS = server client proxy
SERVER_DEPENDENCIES = server.o utility.o
CLIENT_DEPENDENCIES = client.o utility.o
PROXY_DEPENDENCIES = proxy.o utility.o

all: $(TARGETS)

server: $(SERVER_DEPENDENCIES)
	$(CC) $(LFLAGS) $(SERVER_DEPENDENCIES) -o server

client: $(CLIENT_DEPENDENCIES)
	$(CC) $(LFLAGS) $(CLIENT_DEPENDENCIES) -o client

proxy: $(PROXY_DEPENDENCIES)
	$(CC) $(LFLAGS) $(PROXY_DEPENDENCIES) -o proxy

server.o: server.c utility.h
	$(CC) $(CFLAGS) server.c

utility.o: utility.c utility.h
	$(CC) $(CFLAGS) utility.c

client.o: client.c utility.h
	$(CC) $(CFLAGS) client.c

proxy.o: proxy.c  utility.h
	$(CC) $(CFLAGS) proxy.c

clean:
	rm -rf *o
	rm $(TARGETS)
