CC=gcc
CFLAGS=-pipe
all: sim.o
	$(CC) $(CFLAGS) sim.o -o sim -ggdb
myarr.o: sim.c
	$(CC) $(CFLAGS) -c sim.c
clean:
	rm sim
	rm sim.o
