#include <stdio.h>
#include "../include/mmu.h"

// // The Memory Management Unit
// typedef struct MemoryManagementUnit {
//     BYTE bios[0x100];
//     union {
//         BYTE mem[0x10000];
//         struct {
//             // ROM bank 0 and 1
//             BYTE rom[2][0x4000];
//             // Video RAM
//             BYTE vram[0x2000];
//             // External RAM
//             BYTE eram[0x2000];
//             // Work RAM
//             BYTE wram[0x2000];
//             BYTE wrams[0x1E00];
//             // Sprite attribute table
//             BYTE oam[0xA0];
//             // Unusable
//             BYTE empty[0x60];
//             // IO registers
//             BYTE io[0x80];
//             // High RAM
//             BYTE hram[0x80];
//             // Interrupt register
//             BYTE Interrupts;
//         };
//     };
// } MMU;

// // Read a byte from memory
// BYTE read(MMU* mmu, WORD addr) {
//     return mmu->mem[addr];
// }
// 
// // Read a word from memory
// WORD read_word(WORD addr) {
//     return bytes_to_word(mem[addr], mem[addr + 1]);
// }
// 
// // Read the byte at the current memory address and increment
// // the program counter
// BYTE read_next() {
//     return mem[pc++];
// }
// 
// // Read two bytes from the current memory address and increment
// // the program counter twice
// WORD read_next_word() {
//     return bytes_to_word(mem[pc++], mem[pc++]);;
// }
// 
// void write(WORD addr, BYTE data) {
//     mem[addr] = data;
// }
// 
// void write_word(WORD addr, WORD data) {
//     mem[addr] = data;
// }
