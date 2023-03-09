# makefile
# Cameron L Burroughs
# burrou5
# C17134631
# ECE 2230 Fall 2020
# MP6

CC = gcc
CFLAGS = -g -Wall
CLIBS = -lm

lab6 : graph.o lab6.o
	$(CC) $(CFLAGS) graph.o lab6.o -o lab6 $(CLIBS)
graph.o: graph.c graph.h
	$(CC) $(CFLAGS) -c graph.c
lab6.o: lab6.c graph.h
	$(CC) $(CFLAGS) -c lab6.c
clean:
	rm -f *.o lab6 core
