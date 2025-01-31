CC = gcc
CFLAGS = -g -Wall -std=c99

all: csim

csim: src/cachesim.c src/functions.c src/header.h
	$(CC) $(CFLAGS) -o cachesim src/cachesim.c src/functions.c -lm 
clean:
	rm -rf *.o
	rm -rf *.tmp
	rm -f cachesim 
	rm -f trace.all trace.f*

