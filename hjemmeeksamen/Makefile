cc = gcc
CFLAGS = -Wall -g

all: client server

server: server.c protocol.c
	$(CC) $(CFLAGS) -o server server.c protocol.c

client: client.c protocol.c
	$(CC) $(CFLAGS) -o client client.c protocol.c

clean:
	rm -f server client protocol
