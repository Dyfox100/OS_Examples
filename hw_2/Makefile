CC = gcc
CFLAGS = -Wall -g -o

all:
	$(CC) $(CFLAGS) problem_1.out main.c file_utils.c
	$(CC) $(CFLAGS) parent.out problem_2.c
	$(CC) $(CFLAGS) child.out child.c
	$(CC) $(CFLAGS) print.out print_string.c

clean:
	rm -f *.out
