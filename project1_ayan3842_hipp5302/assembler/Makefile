CC=gcc
CFLAGS=-pipe

all: asm.o
	$(CC) $(CFLAGS)  asm.o -o asm -ggdb
myarr.o: asm.c
	$(CC) $(CFLAGS)  -c asm.c
clean:
	rm asm
	rm asm.o
