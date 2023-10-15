CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -g
LOAD=load_balancer
SERVER=server

.PHONY: build clean

build: tema2

tema2: main.o $(LOAD).o $(SERVER).o hash.o
	$(CC) $^ -o $@ 

main.o: main.c
	$(CC) $(CFLAGS) $^ -c

$(SERVER).o: $(SERVER).c $(SERVER).h
	$(CC) $(CFLAGS) $^ -c

$(LOAD).o: $(LOAD).c $(LOAD).h
	$(CC) $(CFLAGS) $^ -c

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) $^ -c

pack:
	zip -r Stefi_tema2.zip *.c *.h Makefile

clean:
	rm -f *.o tema2 *.h.gch
