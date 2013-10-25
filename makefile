CC=g++
CFLAGS=-g -std=gnu++11

all: dispatcher.o lib-ult.o
	$(CC) $(CFLAGS) lib-ult.o dispatcher.o -o ult

test: dispatcher.o test2.o
	$(CC) $(CFLAGS) test2.o dispatcher.o -o test

test2.o: test2.cpp
	$(CC) $(CFLAGS) -c test2.cpp

lib-ult.o: lib-ult.cpp
	$(CC) $(CFLAGS) -c lib-ult.cpp

dispatcher.o:
	$(CC) $(CFLAGS) -c dispatcher.cpp
clean: 
	rm -f ult *.o test
