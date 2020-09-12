CC = gcc
CFLAGS = -o0 -Wall -g
LIBS = -pthread

all: prob_1 prob_2

prob_1: 
	$(CC) $(CFLAGS) $(LIBS) -o problem_1 Problem_1.c
	$(CC) $(CFLAGS) $(LIBS) -o child Child_Processes.c

prob_2:
	$(CC) $(CFLAGS) $(LIBS) -o problem_2 Problem_2.c
clean:
	rm -f problem_1 child problem_2


