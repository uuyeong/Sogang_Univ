CC = gcc
CFLAGS = -Wall -Wextra -g

OBJS = main.o list.o hash.o bitmap.o debug.o hex_dump.o

all: testlib

testlib: $(OBJS)
	$(CC) $(CFLAGS) -o testlib $(OBJS)

main.o: main.c list.h hash.h bitmap.h debug.h hex_dump.h
	$(CC) $(CFLAGS) -c main.c

list.o: list.c list.h
	$(CC) $(CFLAGS) -c list.c

hash.o: hash.c hash.h
	$(CC) $(CFLAGS) -c hash.c

bitmap.o: bitmap.c bitmap.h
	$(CC) $(CFLAGS) -c bitmap.c

debug.o: debug.c debug.h
	$(CC) $(CFLAGS) -c debug.c

hex_dump.o: hex_dump.c hex_dump.h
	$(CC) $(CFLAGS) -c hex_dump.c

clean:
	rm -f testlib $(OBJS)
