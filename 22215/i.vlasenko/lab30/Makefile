all: server client
server: server.c
	gcc server.c -o server -lsocket
client: client.c
	gcc client.c -o client -lsocket
clean:
	rm server client
