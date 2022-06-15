CC = gcc
CFLAGS = -Wall -g
LIB = -pthread

all: prime_sem.o my_sem.o
	$(CC) $(CFLAGS) prime_sem.o my_sem.o -o prime_sem $(LIB)


my_sem.o: my_sem.c my_sem.h
	$(CC) $(CFLAGS) -c my_sem.c

clean:
	rm -f *.o prime_sem


