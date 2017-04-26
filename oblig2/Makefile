cc = gcc
CFLAGS = -Wall -g

all: main

main: main.c safefork.c history.c	
	$(CC) $(CFLAGS) -o main main.c safefork.c history.c

DEBUG: main.c safefork.c history.c
	$(CC) $(CFLAGS) -DDEBUG -o main_DEBUG main.c safefork.c history.c  

clean:
	rm -f main history	
