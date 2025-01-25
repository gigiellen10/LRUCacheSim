CC = gcc
CFLAGS = -g -Wall -Werror -std=c99

all: csim

csim: cachesim.c functions.c header.h
	$(CC) $(CFLAGS) -o p cachesim.c functions.c -lm 

#
# cleanup, remember to change: flag after -o change back to cachesim
# also change rm -f p to rm -f cachesim after doing the above
#
clean:
	rm -rf *.o
	rm -rf *.tmp
	rm -f p 
	rm -f trace.all trace.f*

