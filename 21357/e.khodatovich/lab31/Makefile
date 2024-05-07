all: server.out client.out

server.out: server_proc.c
	cc server_proc.c -v -o server.out -lsocket

client.out: client_proc.c
	cc client_proc.c -v -o client.out -lsocket
