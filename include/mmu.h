#ifndef __MMU_H_
#define __MMU_H_ 1

#include <stdbool.h> 
#include "utils.h"

#define MEM_SIZE 0x10000

struct test_st
{
   int state;
   int status;
};

struct MemoryManagementUnit {
    BYTE bios[0x100];
    union {
        BYTE mem[0x10000];
        struct {
            // ROM bank 0 and 1
            BYTE rom[2][0x4000];
            // Video RAM
            BYTE vram[0x2000];
            // External RAM
            BYTE eram[0x2000];
            // Work RAM
            BYTE wram[0x2000];
            BYTE wrams[0x1E00];
            // Sprite attribute table
            BYTE oam[0xA0];
            // Unusable
            BYTE empty[0x60];
            // IO registers
            BYTE io[0x80];
            // High RAM
            BYTE hram[0x80];
            // Interrupt register
            BYTE Interrupts;
        };
    };
};

// BYTE read(WORD addr);
// WORD read_word(WORD addr);
// void write(WORD addr, BYTE data);
// void write_word(WORD addr, WORD data);

#endif
