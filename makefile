CC=gcc
CFLAGS=-I.
DEPS=gameboy.h

gb:
	$(CC) -o gameboy main.c cpu.c mmu.c $(CFLAGS)
clean: 
	rm ./gameboy
