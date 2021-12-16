#include <stdbool.h> 
#include <stdio.h>
#include "../include/mmu.h"

BYTE mem[MEM_SIZE];

// Read a byte from memory
BYTE read(WORD addr) {
    return mem[addr];
}

// Read a word from memory
WORD read_word(WORD addr) {
    return bytes_to_word(mem[addr], mem[addr + 1]);
}

// Read the byte at the current memory address and increment
// the program counter
BYTE read_next() {
    return mem[pc++];
}

// Read two bytes from the current memory address and increment
// the program counter twice
WORD read_next_word() {
    return bytes_to_word(mem[pc++], mem[pc++]);;
}

void write(WORD addr, BYTE data) {
    mem[addr] = data;
}

void write_word(WORD addr, WORD data) {
    mem[addr] = data;
}
