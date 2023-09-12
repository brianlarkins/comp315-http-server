#
# Makefile for lab 2
#
CC=clang
CFLAGS=-g

httpserver: httpserver.o mime.o comm.o
	clang -o $@ $^

clean:
	rm -f httpserver *.o
	rm -fr *.dSYM
