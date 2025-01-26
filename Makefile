CC = gcc
CFLAGS = -g -Wall -std=c99

all: csim

csim: src/cachesim.c src/functions.c src/header.h
	$(CC) $(CFLAGS) -o p src/cachesim.c src/functions.c -lm 

#
# cleanup, remember to change: flag after -o change back to cachesim
# also change rm -f p to rm -f cachesim after doing the above
#
clean:
	rm -rf *.o
	rm -rf *.tmp
	rm -f p 
	rm -f trace.all trace.f*

