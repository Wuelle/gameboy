#include <stdbool.h> 
#include <stdio.h>
#include "../include/mmu.h"

BYTE mem[MEM_SIZE];
BYTE registers[NUM_REGISTERS];
WORD sp;
WORD pc = 0;

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

BYTE get_register(BYTE ix) {
    return registers[ix];
}

void set_register(BYTE ix, BYTE val) {
    registers[ix] = val;
}

WORD get_16b_register(BYTE ix1, BYTE ix2) {
    BYTE reg_1 = registers[ix1];
    BYTE reg_2 = registers[ix2];
    return bytes_to_word(reg_1, reg_2);
}

void set_16b_register(char ix1, char ix2, WORD data ){
    registers[ix1] = data >> 8;
    registers[ix2] = data & 0x00FF;
}

bool get_flag(BYTE ix) {
    return (registers[F] >> ix) & 1 == 1;
}

void set_flag(BYTE ix) {
    registers[F] |= 1 << ix;
}

void set_flag_to(BYTE ix, bool to) {
    if (to) {
        set_flag(ix);
    } else {
        unset_flag(ix);
    }
}

void unset_flag(BYTE ix) {
    registers[F] &= ~(1 << ix);
}
