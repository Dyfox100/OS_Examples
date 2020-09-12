CC = gcc
CFLAGS = -Wall -g
LIBS = -pthread

all: prob_1 prob_2

prob_1: 
	$(CC) $(CFLAGS) $(LIBS) -o problem_1 Problem_1.c
	$(CC) $(CFLAGS) $(LIBS) -o child Child_Processes.c
	
prob_2:

clean:
	rm -f problem_1 child


