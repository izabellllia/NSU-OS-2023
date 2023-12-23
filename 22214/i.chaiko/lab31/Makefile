all: server client

server: server.c
    gcc -Wall -lsocket -lnsl server.c -o server

client: client.c
    gcc -Wall -lsocket -lnsl client.c -o client

clean:
    rm server client 
