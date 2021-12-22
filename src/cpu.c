#include <stdio.h>
#include <stdbool.h>
#include "../include/mmu.h"
#include "../include/cpu.h"

#define FLAG_C 4
#define FLAG_H 5
#define FLAG_N 6
#define FLAG_Z 7

extern BYTE mem[];

struct Processor {
    bool is_halted;
    bool is_stopped;

    // Both IE and DE only take effect after one cycle 
    bool enable_interrupts_instruction;
    bool disable_interrupts_instruction;

    WORD SP;
    WORD PC = 0;

    // Register declarations
    union {
        struct {
            BYTE F;
            BYTE A;
        };
        WORD AF;
    };
    union {
        struct {
            BYTE C;
            BYTE B;
        };
        WORD BC;
    };
    union {
        struct {
            BYTE E;
            BYTE D;
        };
        WORD DE;
    };
    union {
        struct {
            BYTE F;
            BYTE H;
        };
        WORD HF;
    };
}

BYTE read_next();
WORD read_next_word();

bool get_flag(Processor* cpu, BYTE ix) {
    return (cpu->F >> ix) & 1 == 1;
}

void set_flag(BYTE ix) {
    cpu->F |= 1 << ix;
}

void set_flag_to(Processor* cpu, BYTE ix, bool to) {
    if (to) {
        set_flag(cpu, ix);
    } else {
        unset_flag(cpu, ix);
    }
}

void unset_flag(Processor* cpu, BYTE ix) {
    cpu->F &= ~(1 << ix);
}

BYTE add_with_flags_u8(Processor* cpu, BYTE a, BYTE b, bool add_carry, bool affect_carry) {
    // Reset all the flags
    cpu->F = 0;

    if(add_carry) {
        if(get_flag(cpu, FLAG_C)) {
            b++;
        }
    }

    BYTE res = a + b;

    // Set the carry flag accordingly
    if(affect_carry) {
        set_flag_to(cpu, FLAG_C, res < a);
    }

    // Calculate the half carry flag
    set_flag_to(cpu, FLAG_H, (a & 0xF) + (b & 0xF) > 0xF);

    return res;
}

WORD add_with_flags_u16(Processor* cpu, WORD a, WORD b) {
    WORD res = a + b;

    // Check for an overflow
    if(res < a) {
        set_flag(cpu, FLAG_C);
    }

    unset_flag(cpu, FLAG_N);

    // Calculate the half carry flag
    set_flag_to(FLAG_H, (a & 0xFFF) + (b & 0xFFF) > 0xFFF);

    return res;
}

BYTE sub_with_flags_u8(Processor* cpu, BYTE a, BYTE b, bool sub_carry, bool affect_carry) {
    // Reset all the flags
    cpu->F = 0;
    set_flag(cpu, FLAG_N);

    // Set the carry flag accordingly
    if(sub_carry) {
        if(get_flag(cpu, FLAG_C)) {
            b++;
        }
    }

    BYTE res = a - b;

    if(affect_carry) {
        set_flag_to(cpu, FLAG_C, res > a);
    }

    // Calculate the half carry flag
    SIGNED_BYTE a_ = a & 0xF;
    set_flag_to(cpu, FLAG_H, a_ - (b & 0xF) < 0);

    return res;
}
// Test bit b in register reg
void BIT(cpu, int reg, int b) {
    if (((get_register(val) >> b) & 1) == 0) {
        set_flag(FLAG_Z);
    }
    unset_flag(FLAG_N);
    set_flag(FLAG_H);
}

// Set bit b in 8 bit register reg
void SET(int reg, int b) {
    reg |= 1 << b;
}

// Set bit b in 16 bit register A, B
void SET(int A, int B, int b) {
    WORD res = get_16b_register(A, B) | (1 << b);
    set_16b_register(res);
}

// Reset bit b in register reg
void RES(int reg, int b) {
    reg &= ~(1 << b);
}


// Logical AND with register A, result in A.
void AND(Processor* cpu, BYTE val) {
    cpu->A = cpu->A & val;
    set_flag_to(cpu, FLAG_Z, cpu->A == 0);
    unset_flag(cpu, FLAG_N);
    set_flag(cpu, FLAG_H);
    unset_flag(cpu, FLAG_C);
}

// Logical OR with register A, result in A.
void OR(Processor* cpu, BYTE val) {
    cpu->A = cpu->A | val;
    cpu->F = 0;
    set_flag_to(cpu, FLAG_Z, cpu->A == 0);
}

// Logical exclusive OR with register A, result in A
void XOR(BYTE val) {
    cpu->A = cpu->A ^ val;
    cpu->F = 0;
    set_flag_to(cpu, FLAG_Z, cpu->A == 0);
}

// Rotate left. Old bit 7 to Carry flag.
BYTE RLC(BYTE val) {
    BYTE res =(val << 1) | (val >> 7);
    set_flag_to(FLAG_C, (val >> 7) & 1);
    if(res == 0) {
        set_flag(FLAG_Z);
    }
    unset_flag(FLAG_N);
    unset_flag(FLAG_H);
    return res;
}
// Swap upper and lower nibles.
BYTE SWAP(BYTE val) {
    set_register(F, 0); // reset all flags
    res = (val >> 4) | (val << 4);
    set_flag_to(FLAG_Z, res == 0);
    return res;
}

// Rotate left through the Carry flag.
BYTE RL(BYTE val) {
    BYTE res = (val << 1) | get_flag(FLAG_C);
    set_flag_to(FLAG_C, (val >> 7) & 1);
    if(res == 0) {
        set_flag(FLAG_Z);
    }
    unset_flag(FLAG_N);
    unset_flag(FLAG_H);
    return res;
}

// Rotate right through the Carry flag.
BYTE RR(BYTE val) {
    BYTE res = (val >> 1) | (get_flag(FLAG_C) << 7);
    set_flag_to(FLAG_C, val & 1);

    if(res == 0) {
        set_flag(FLAG_Z);
    }
    unset_flag(FLAG_N);
    unset_flag(FLAG_H);
    return res;
}
// Rotate right. Old bit 0 to Carry flag
BYTE RRC(BYTE val) {
    BYTE res = (val >> 1) | ((val & 0x01) << 7);
    set_flag_to(FLAG_C, val & 0x01); // last bit
    if(res == 0) {
        set_flag(FLAG_Z);
    }
    unset_flag(FLAG_N);
    unset_flag(FLAG_H);
    return res;
}

// Shift left into Carry. LSB of n set to 0.
BYTE SLA(BYTE val) {
    BYTE res = val << 1;
    unset_flag(FLAG_N);
    unset_flag(FLAG_H);
    set_flag_to(FLAG_C, (val >> 7) & 1);
    set_flag_to(FLAG_Z, res == 0);
    return res;
}
// Shift right into Carry. MSB does not change.
BYTE SRA(BYTE val) {
    BYTE res = (val >> 1) | (val & 0x100);
    set_flag_to(FLAG_Z, res == 0);
    unset_flag(FLAG_N);
    unset_flag(FLAG_H);
    set_flag_to(FLAG_C, val & 1);
    return res;
}

// Shift right into Carry. MSB set to 0.
BYTE SRL(BYTE val) {
    set_flag_to(FLAG_Z, (val >> 1) == 0);
    unset_flag(FLAG_N);
    unset_flag(FLAG_H);
    set_flag_to(FLAG_C, val & 1);
    return val >> 1;
}

// Execute the next instruction, increment the program counter and return the 
// number of simulated clock cycles 
int execute_next(Processor* cpu) {
    switch(read_next()) {
        BYTE val, val_2, res;
        WORD addr;
        // LD B, n
        case 0x06:
            cpu->B = read_next();
            return 8;
        // LD C, n
        case 0x0E:
            cpu->C = read_next();
            return 8;
        // LD D, n
        case 0x16:
            cpu->D = read_next();
            return 8;
        // LD E, n
        case 0x1E:
            cpu->E = read_next();
            return 8;
        // LD H, n
        case 0x26:
            cpu->H = read_next();
            return 8;
        // LD L, n
        case 0x2E:
            cpu->L = read_next();
            return 8;
        // LD A, A
        case 0x7F:
            cpu->A = read_next();
            return 4;
        // LD A, B
        case 0x78:
            cpu->A = cpu->B;
            return 4;
        // LD A, C
        case 0x79:
            cpu->A = cpu->C;
            return 4;
        // LD A, D
        case 0x7A:
            cpu->A = cpu->D;
            return 4;
        // LD A, E
        case 0x7B:
            cpu->A = cpu->E;
            return 4;
        // LD A, H
        case 0x7C:
            cpu->A = cpu->H;
            return 4;
        // LD A, L
        case 0x7D:
            cpu->A = cpu->L;
            return 4;
        // LD A, <BC>
        case 0x0A:
            cpu->A = read(cpu->BC);
            return 8;
        // LD A, <DE>
        case 0x1A:
            cpu->A = read(cpu->DE);
            return 8;
        // LD A, <HL>
        case 0x7E:
            cpu->A = read(cpu->HL);
            return 8;
        // LD A, <nn>
        case 0xFA:
            cpu->A = read(read_next_word());
            return 16;
        // LD A, #
        case 0x3E:
            cpu->A = read_next();
            return 8;
        // LD B, A
        case 0x47:
            cpu->B = cpu->A;
            return 4;
        // LD B, B
        case 0x40:
            cpu->B = cpu->B;
            return 4;
        // LD B, C
        case 0x41:
            cpu->B = cpu->C;
            return 4;
        // LD B, D
        case 0x42:
            cpu->B = cpu->D;
            return 4;
        // LD B, E
        case 0x43:
            cpu->B = cpu->E;
            return 4;
        // LD B, H
        case 0x44:
            cpu->B = cpu->H;
            return 4;
        // LD B, L
        case 0x45:
            cpu->B = cpu->L;
            return 4;
        // LD B, <HL>
        case 0x46:
            cpu->B = read(cpu->HL);
            return 8;
        // LD C, A
        case 0x4F:
            cpu->C = cpu->A;
            return 4;
        // LD C, B
        case 0x48:
            cpu->C = cpu->B;
            return 4;
        // LD C, C
        case 0x49:
            cpu->C = cpu->C;
            return 4;
        // LD C, D
        case 0x4A:
            cpu->C = cpu->D;
            return 4;
        // LD C, E
        case 0x4B:
            cpu->C = cpu->E;
            return 4;
        // LD C, H
        case 0x4C:
            cpu->C = cpu->H;
            return 4;
        // LD C, L
        case 0x4D:
            cpu->C = cpu->L;
            return 4;
        // LD C, <HL>
        case 0x4E:
            cpu->C = read(cpu->HL);
            return 8;
        // LD D, A
        case 0x57:
            cpu->D = cpu->A;
            return 4;
        // LD D, B
        case 0x50:
            cpu->D = cpu->B;
            return 4;
        // LD D, C
        case 0x51:
            cpu->D = cpu->C;
            return 4;
        // LD D, D
        case 0x52:
            cpu->D = cpu->D;
            return 4;
        // LD D, E
        case 0x53:
            cpu->D = cpu->E;
            return 4;
        // LD D, H
        case 0x54:
            cpu->D = cpu->H;
            return 4;
        // LD D, L
        case 0x55:
            cpu->D = cpu->L;
            return 4;
        // LD D, <HL>
        case 0x56:
            cpu->D = read(cpu->HL);
            return 8;
        // LD E, A
        case 0x5F:
            cpu->E = cpu->A;
            return 4;
        // LD E, B
        case 0x58:
            cpu->E = cpu->B;
            return 4;
        // LD E, C
        case 0x59:
            cpu->E = cpu->C;
            return 4;
        // LD E, D
        case 0x5A:
            cpu->E = cpu->D;
            return 4;
        // LD E, E
        case 0x5B:
            cpu->E = cpu->E;
            return 4;
        // LD E, H
        case 0x5C:
            cpu->E = cpu->H;
            return 4;
        // LD E, L
        case 0x5D:
            cpu->E = cpu->L;
            return 4;
        // LD E, <HL>
        case 0x5E:
            cpu->E = read(cpu->HL);
            return 8;
        // LD H, A
        case 0x67:
            cpu->H = cpu->A;
            return 4;
        // LD H, B
        case 0x60:
            cpu->H = cpu->B;
            return 4;
        // LD H, C
        case 0x61:
            cpu->H = cpu->C;
            return 4;
        // LD H, D
        case 0x62:
            cpu->H = cpu->D;
            return 4;
        // LD H, E
        case 0x63:
            cpu->H = cpu->E;
            return 4;
        // LD H, H
        case 0x64:
            cpu->H = cpu->H;
            return 4;
        // LD H, L
        case 0x65:
            cpu->H = cpu->L;
            return 4;
        // LD H, <HL>
        case 0x66:
            cpu->H = read(cpu->HL);
            return 8;
        // LD L, A
        case 0x6F:
            cpu->L = cpu->A;
            return 4;
        // LD L, B
        case 0x68:
            cpu->L = cpu->B;
            return 4;
        // LD L, C
        case 0x69:
            cpu->L = cpu->C;
            return 4;
        // LD L, D
        case 0x6A:
            cpu->L = cpu->D;
            return 4;
        // LD L, E
        case 0x6B:
            cpu->L = cpu->E;
            return 4;
        // LD L, H
        case 0x6C:
            cpu->L = cpu->H;
            return 4;
        // LD L, L
        case 0x6D:
            cpu->L = cpu->L;
            return 4;
        // LD L, <HL>
        case 0x6E:
            cpu->L = read(cpu->HL);
            return 8;
        // LD <HL>, B
        case 0x70:
            write(cpu->HL, cpu->B);
            return 8;
        // LD <HL>, C
        case 0x71:
            write(cpu->HL, cpu->C);
            return 8;
        // LD <HL>, D
        case 0x72:
            write(cpu->HL, cpu->D);
            return 8;
        // LD <HL>, E
        case 0x73:
            write(cpu->HL, cpu->E);
            return 8;
        // LD <HL>, H
        case 0x74:
            write(cpu->HL, cpu->H);
            return 8;
        // LD <HL>, L
        case 0x75:
            write(cpu->HL, cpu->L);
            return 8;
        // LD <HL>, n
        case 0x36:
            write(cpu->HL, read_next());
            return 12;
        // LD <BC>, A
        case 0x02:
            write(cpu->BC, cpu->A);
            return 8;
        // LD <DE>, A
        case 0x12:
            write(cpu->DE, cpu->A);
            return 8;
        // LD <HL>, A
        case 0x77:
            write(cpu->HL, cpu->A);
            return 8;
        // LD <nn>, A
        case 0xEA:
            write(read_next_word(), cpu->A);
            return 16;
        // LD A, <0xFF00 + C>
        case 0xF2:
            cpu->A = read(0xFF00 + cpu->C);
            return 8;
        // LD <0xFF00 + C>
        case 0xE2:
            write(0xFF00 + cpu->C, cpu->A);
            return 8;
        // LDD A, <HL>
        case 0x3A:
            cpu->A = read(cpu->HL);
            cpu->HL--;
            return 8;
        // LDD <HL>, A
        case 0x32:
            write(cpu->HL, cpu->A);
            cpu->HL--;
            return 8;
        // LDI A, <HL>
        case 0x2A:
            cpu->A = read(cpu->HL);
            cpu->HL++;
            return 8;
        // LDI <HL>, A
        case 0x22:
            write(cpu->HL, cpu->A);
            cpu->HL++;
            return 8;
        // LDH <0xFF00 + n>, A
        case 0xE0:
            addr = 0xFF00 + read_next();
            write(addr, cpu->A);
            return 12;
        // LDH A, <0xFF00 + n>
        case 0xF0:
            cpu->A = read(0xFF00 + read_next());
            return 12;
        // LD BC, nn
        case 0x01:
            cpu->BC = read_next_word();
            return 12;
        // LD DE, nn
        case 0x11:
            cpu->DE = read_next_word();
            return 12;
        // LD HL, nn
        case 0x21:
            cpu->HL = read_next_word();
            return 12;
        // LD SP, nn
        case 0x31:
            cpu->SP = read_next_word();
            return 12;
        // LD SP, HL
        case 0xF9:
            cpu->SP = cpu->HL;
            return 8;
        // LDHL SP, n
        case 0xF8:
            val = read_next();
            // TODO: there is probably a bug with val_2 here
            // Hacky was to set the flags as if it was an 8bit operation
            add_with_flags_u8((BYTE)cpu->SP, val_2, false, true);

            cpu->HL = cpu->SP + val;
            // This instruction resets the zero flag
            unset_flag(cpu, FLAG_Z);
            return 12;
        // LD <nn>, SP
        case 0x08:
            addr = read_next_word();
            write_word(addr, cpu->SP);
            return 20;
        // PUSH AF
        case 0xF5:
            write_word(cpu->SP, cpu->AF);
            cpu->SP -= 2;
            return 16;
        // PUSH BC
        case 0xC5:
            write_word(cpu->SP, cpu->BC);
            cpu->SP -= 2;
            return 16;
        // PUSH DE
        case 0xD5:
            write_word(cpu->SP, cpu->DE);
            cpu->SP -= 2;
            return 16;
        // PUSH HL
        case 0xE5:
            write_word(cpu->SP, cpu->HL);
            cpu->SP -= 2;
            return 16;
        // POP AF
        case 0xF1:
            cpu->AF = read_word(cpu->SP);
            cpu->SP += 2;
            return 12;
        // POP BC
        case 0xC1:
            cpu->BC = read_word(cpu->SP);
            cpu->SP += 2;
            return 12;
        // POP DE
        case 0xD1:
            cpu->DE = read_word(cpu->SP);
            cpu->SP += 2;
            return 12;
        // POP HL
        case 0xE1:
            cpu->HL = read_word(cpu->SP);
            cpu->SP += 2;
            return 12;
        // ADD A, A
        case 0x87:
            cpu->A = add_with_flags_u8(cpu->A, cpu->A, false, true);
            return 4;
        // ADD A, B
        case 0x80:
            cpu->A = add_with_flags_u8(cpu->A, cpu->B, false, true);
            return 4;
        // ADD A, C
        case 0x81:
            cpu->A = add_with_flags_u8(cpu->A, cpu->C, false, true);
            return 4;
        // ADD A, D
        case 0x82:
            cpu->A = add_with_flags_u8(cpu->A, cpu->D, false, true);
            return 4;
        // ADD A, E
        case 0x83:
            cpu->A = add_with_flags_u8(cpu->A, cpu->E, false, true);
            return 4;
        // ADD A, H
        case 0x84:
            cpu->A = add_with_flags_u8(cpu->A, cpu->H, false, true);
            return 4;
        // ADD A, L
        case 0x85:
            cpu->A = add_with_flags_u8(cpu->A, cpu->L, false, true);
            return 4;
        // ADD A, <HL>
        case 0x86:
            cpu->A = add_with_flags_u8(cpu->A, read(cpu->HL), false, true);
            return 8;
        // ADD A, #
        case 0xC6:
            cpu->A = add_with_flags_u8(cpu->A, read_next(), false, true);
            return 8;
        // ADC A, A
        case 0x8F:
            cpu->A = add_with_flags_u8(cpu->A, cpu->A, true, true);
            return 4;
        // ADC A, B
        case 0x88:
            cpu->A = add_with_flags_u8(cpu->A, cpu->B, true, true);
            return 4;
        // ADC A, C
        case 0x89:
            cpu->A = add_with_flags_u8(cpu->A, cpu->C, true, true);
            return 4;
        // ADC A, D
        case 0x8A:
            cpu->A = add_with_flags_u8(cpu->A, cpu->D, true, true);
            return 4;
        // ADC A, E
        case 0x8B:
            cpu->A = add_with_flags_u8(cpu->A, cpu->E, true, true);
            return 4;
        // ADC A, H
        case 0x8C:
            cpu->A = add_with_flags_u8(cpu->A, cpu->H, true, true);
            return 4;
        // ADC A, L
        case 0x8D:
            cpu->A = add_with_flags_u8(cpu->A, cpu->L, true, true);
            return 4;
        // ADC A, <HL>
        case 0x8E:
            cpu->A = add_with_flags_u8(cpu->A, read(cpu->HL), true, true);
            return 8;
        // ADC A, #
        case 0xCE:
            cpu->A = add_with_flags_u8(cpu->A, read_next(), true, true);
            return 8;
        // SUB A, A
        case 0x97:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->A, false, true);
            return 4;
        // SUB A, B
        case 0x90:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->B, false, true);
            return 4;
        // SUB A, C
        case 0x91:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->C, false, true);
            return 4;
        // SUB A, D
        case 0x92:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->D, false, true);
            return 4;
        // SUB A, E
        case 0x93:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->E, false, true);
            return 4;
        // SUB A, H
        case 0x94:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->H, false, true);
            return 4;
        // SUB A, L
        case 0x95:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->L, false, true);
            return 4;
        // SUB A, <HL>
        case 0x96:
            cpu->A = sub_with_flags_u8(cpu->A, read(cpu->HL), false, true);
            return 8;
        // SUB A, #
        case 0xD6:
            cpu->A = sub_with_flags_u8(cpu->A, read_next(), false, true);
            return 8;
        // SBC A, A
        case 0x9F:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->A, true, true);
            return 4;
        // SBC A, B
        case 0x98:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->B, true, true);
            return 4;
        // SBC A, C
        case 0x99:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->C, true, true);
            return 4;
        // SBC A, D
        case 0x9A:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->D, true, true);
            return 4;
        // SBC A, E
        case 0x9B:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->E, true, true);
            return 4;
        // SBC A, H
        case 0x9C:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->H, true, true);
            return 4;
        // SBC A, L
        case 0x9D:
            cpu->A = sub_with_flags_u8(cpu->A, cpu->L, true, true);
            return 4;
        // SBC A, <HL>
        case 0x9E:
            cpu->A = sub_with_flags_u8(cpu->A, read(cpu->HL), true, true);
            return 8;
        // SBC A, #
        // Unknown Opcode. Probably never used anyways
        // case 0xCE:
        //     cpu->A = sub_with_flags_u8(cpu->A, read_next(), true, true);
        //     return ??;
        // AND A, A
        case 0xA7:
            AND(cpu, cpu->A);
            return 4;
        // AND A, B
        case 0xA0:
            AND(cpu, cpu->B);
            return 4;
        // AND A, C
        case 0xA1:
            AND(cpu, cpu->C);
            return 4;
        // AND A, D
        case 0xA2:
            AND(cpu, cpu->D);
            return 4;
        // AND A, E
        case 0xA3:
            AND(cpu, cpu->E);
            return 4;
        // AND A, H
        case 0xA4:
            AND(cpu, cpu->H);
            return 4;
        // AND A, L
        case 0xA5:
            AND(cpu, cpu->L);
            return 4;
        // AND A, <HL>
        case 0xA6:
            AND(cpu, read(cpu->HL));
            return 8;
        // AND A, #
        case 0xE6:
            AND(cpu, read_next());
            return 8;
        // OR A, A
        case 0xB7:
            OR(cpu, cpu->A);
            return 4;
        // OR A, B
        case 0xB0:
            OR(cpu, cpu->B);
            return 4;
        // OR A, C
        case 0xB1:
            OR(cpu, cpu->C);
            return 4;
        // OR A, D
        case 0xB2:
            OR(cpu, cpu->D);
            return 4;
        // OR A, E
        case 0xB3:
            OR(cpu, cpu->E);
            return 4;
        // OR A, H
        case 0xB4:
            OR(cpu, cpu->H);
            return 4;
        // OR A, L
        case 0xB5:
            OR(cpu, cpu->L);
            return 4;
        // OR A, <HL>
        case 0xB6:
            OR(cpu, read(cpu->HL));
            return 8;
        // OR A, #
        case 0xF6:
            OR(cpu, read_next());
            return 8;
        // XOR A, A
        case 0xAF:
            XOR(cpu, cpu->A);
            return 4;
        // XOR A, B
        case 0xA8:
            XOR(cpu, cpu->B);
            return 4;
        // XOR A, C
        case 0xA9:
            XOR(cpu, cpu->C);
            return 4;
        // XOR A, D
        case 0xAA:
            XOR(cpu, cpu->D);
            return 4;
        // XOR A, E
        case 0xAB:
            XOR(cpu, cpu->E);
            return 4;
        // XOR A, H
        case 0xAC:
            XOR(cpu, cpu->H);
            return 4;
        // XOR A, L
        case 0xAD:
            XOR(cpu, cpu->L);
            return 4;
        // XOR A, <HL>
        case 0xAE:
            XOR(cpu, read(cpu->HL));
            return 8;
        // XOR A, #
        case 0xEE:
            XOR(cpu, read_next());
            return 8;
        // CP A, A
        case 0xBF:
            sub_with_flags_u8(cpu->A, cpu->A, false, true);
            return 4;
        // CP A, B
        case 0xB8:
            sub_with_flags_u8(cpu->A, cpu->B, false, true);
            return 4;
        // CP A, C
        case 0xB9:
            sub_with_flags_u8(cpu->A, cpu->C, false, true);
            return 4;
        // CP A, D
        case 0xBA:
            sub_with_flags_u8(cpu->A, cpu->D, false, true);
            return 4;
        // CP A, E
        case 0xBB:
            sub_with_flags_u8(cpu->A, cpu->E, false, true);
            return 4;
        // CP A, H
        case 0xBC:
            sub_with_flags_u8(cpu->A, cpu->H, false, true);
            return 4;
        // CP A, L
        case 0xBD:
            sub_with_flags_u8(cpu->A, cpu->L, false, true);
            return 4;
        // CP A, <HL>
        case 0xBE:
            sub_with_flags_u8(cpu->A, read(cpu->HL), false, true);
            return 8;
        // CP A, #
        case 0xFE:
            sub_with_flags_u8(cpu->A, read_next(), false, true);
            return 8;
        // INC A
        case 0x3C:
            cpu->A = add_with_flags_u8(cpu->A, 1, false, false);
            return 4;
        // INC B
        case 0x04:
            cpu->B = add_with_flags_u8(cpu->B, 1, false, false);
            return 4;
        // INC C
        case 0x0C:
            cpu->C = add_with_flags_u8(cpu->C, 1, false, false);
            return 4;
        // INC D
        case 0x14:
            cpu->D = add_with_flags_u8(cpu->D, 1, false, false);
            return 4;
        // INC E
        case 0x1C:
            cpu->E = add_with_flags_u8(cpu->E, 1, false, false);
            return 4;
        // INC H
        case 0x24:
            cpu->H = add_with_flags_u8(cpu->H, 1, false, false);
            return 4;
        // INC L
        case 0x2C:
            cpu->L = add_with_flags_u8(cpu->L, 1, false, false);
            return 4;
        // INC <HL>
        case 0x34:
            val = add_with_flags_u8(read(cpu->HL), 1, false, false);
            write(cpu->HL, val);
            return 12;
        // DEC A
        case 0x3D:
            cpu->A = sub_with_flags_u8(cpu->A, 1, false, false);
            return 4;
        // DEC B
        case 0x05:
            cpu->B = sub_with_flags_u8(cpu->B, 1, false, false);
            return 4;
        // DEC C
        case 0x0D:
            cpu->C = sub_with_flags_u8(cpu->C, 1, false, false);
            return 4;
        // DEC D
        case 0x15:
            cpu->D = sub_with_flags_u8(cpu->D, 1, false, false);
            return 4;
        // DEC E
        case 0x1D:
            cpu->E = sub_with_flags_u8(cpu->E, 1, false, false);
            return 4;
        // DEC H
        case 0x25:
            cpu->H = sub_with_flags_u8(cpu->H, 1, false, false);
            return 4;
        // DEC L
        case 0x2D:
            cpu->L = sub_with_flags_u8(cpu->L, 1, false, false);
            return 4;
        // DEC <HL>
        case 0x35:
            val = sub_with_flags_u8(read(cpu->HL), 1, false, false);
            write(cpu->HL, val);
            return 12;
        // ADD HL, BC
        case 0x09:
            cpu->HL = add_with_flags_u16(cpu->HL, cpu->BC);
            return 8;
        // ADD HL, DE
        case 0x19:
            cpu->HL = add_with_flags_u16(cpu->HL, cpu->DE);
            return 8;
        // ADD HL, HL
        case 0x29:
            cpu->HL = add_with_flags_u16(cpu->HL, cpu->HL);
            return 8;
        // ADD HL, SP
        case 0x39:
            cpu->HL = add_with_flags_u16(cpu->HL, cpu->SP);
            return 8;
        // ADD SP, # 
        // TODO: verify this works
        case 0xE8: 
            {
                val = (SIGNED_BYTE)read_next();
                res = cpu->SP + val;

                set_flag_to(cpu, FLAG_H, ((cpu->SP ^ res ^ val) & 0x1000) ? true : false);

                if (val < 0) {
                    set_flag_to(cpu, FLAG_C, res > cpu->SP);
                }
                else {
                    set_flag_to(cpu, FLAG_C, res < cpu->SP);
                }

                // this instruction resets the zero and subtraction flag
                unset_flag(cpu, FLAG_Z);
                unset_flag(cpu, FLAG_N);

                cpu->SP = res;
            }
            return 16;
        // INC BC
        case 0x03:
            cpu->BC++;
            return 8;
        // INC DE
        case 0x13:
            cpu->DE++;
            return 8;
        // INC HL
        case 0x23:
            cpu->HL++;
            return 8;
        // INC SP
        case 0x33:
            cpu->SP++;
            return 8;
        // DEC BC
        case 0x0B:
            cpu->BC--;
            return 8;
        // DEC DE
        case 0x1B:
            cpu->DE--;
            return 8;
        // DEC HL
        case 0x2B: 
            cpu->HL--;
            return 8;
        // DEC SP
        case 0x3B:
            cpu->SP--;
            return 8;
        // DAA
        case 0x27:
            printf("UNIMPLEMENTED: 0x27");
            return 4;
        // CPL
        case 0x2F:
            cpu->A = ~cpu->A;
            set_flag(cpu, FLAG_N);
            set_flag(cpu, FLAG_H);
            return 4;
        // CCF
        case 0x3F:
            unset_flag(cpu, FLAG_N);
            unset_flag(cpu, FLAG_H);
            get_flag(cpu, FLAG_C) ? unset_flag(cpu, FLAG_C) : set_flag(cpu, FLAG_C);
            return 4;
        // SCF
        case 0x37:
            unset_flag(cpu, FLAG_N);
            unset_flag(cpu, FLAG_H);
            set_flag(cpu, FLAG_C);
            return 4;
        // NOP
        case 0x00:
            return 4;
        // HALT
        case 0x76:
            cpu->is_halted = true;
            return 4;
        // DI
        case 0xF3:
            cpu->disable_interrupts_instruction = true;
            return 4;
        // EI
        case 0xFB:
            cpu->enable_interrupts_instruction = true;
            return 4;
        // TODO: continue implementing the new cpu type
        // RLCA
        case 0x07:
            val = get_register(A);
            set_flag_to(FLAG_C, (val >> 7) & 1);
            set_register(A, (val << 1) | (val >> 7));
            if(get_register(A) == 0) {
                set_flag(FLAG_Z);
            }
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            return 4;
        // RLA
        case 0x17:
            val = get_register(A);
            {
                BYTE old_a = val;
                set_register(A, (val << 1) | get_flag(FLAG_C));
                set_flag_to(FLAG_C, (old_a >> 7) & 1);
            }
            if(get_register(A) == 0) {
                set_flag(FLAG_Z);
            }
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            return 4;
        // RRCA
        case 0x0F:
            val = get_register(A);
            set_flag_to(FLAG_C, val & 0x01);
            set_register(A, (val >> 1) | ((val & 0x01) << 7));
            if(get_register(A) == 0) {
                set_flag(FLAG_Z);
            }
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            return 4;
        // RRA
        case 0x1F:
            val = get_register(A);
            set_register(A, (val >> 1) | (get_flag(FLAG_C) << 7));
            set_flag_to(FLAG_C, val & 1);
            if(get_register(A) == 0) {
                set_flag(FLAG_Z);
            }
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            return 4;


        // Extended instruction set 10
        case 0x10:
            return execute_extended_instruction(0x10, read_next());
        // Extended instruction set CB
        case 0xCB:
            return execute_extended_instruction(0xCB, read_next());
        default:
            printf("Unknown Instruction\n");
            return 1;
    }

}

// Execute an instruction from the extended instruction set, returning
// the number of simulated machine cycles
int execute_extended_instruction(BYTE prefix, BYTE op) {
    switch (prefix) {
        case 0x10:
            switch (op) {
                // STOP
                case 0x00:
                    is_halted = true;
                    is_stopped = true;
                    return 4;
                default:
                    printf("Unknown 10 extended Instruction\n");
                    return 1;

            }
            break;
        case 0xCB:
            switch (op) {
                BYTE val, res;
                WORD addr;
                // SWAP A
                case 0x37:
                    val = get_register(A);
                    set_register(A, SWAP(val));
                    return 8;
                // SWAP B
                case 0x30:
                    val = get_register(B);
                    set_register(B, SWAP(val));
                    return 8;
                // SWAP C
                case 0x31:
                    val = get_register(C);
                    set_register(C, SWAP(val));
                    return 8;
                // SWAP D
                case 0x32:
                    val = get_register(D);
                    set_register(D, SWAP(val));
                    return 8;
                // SWAP E
                case 0x33:
                    val = get_register(E);
                    set_register(E, SWAP(val));
                    return 8;
                // SWAP H
                case 0x34:
                    val = get_register(H);
                    set_register(H, SWAP(val));
                    return 8;
                // SWAP L
                case 0x35:
                    val = get_register(L);
                    set_register(L, SWAP(val));
                    return 8;
                // SWAP <HL>
                case 0x36:
                    addr = get_16b_register(H, L);
                    val = read(addr);
                    write(addr, SWAP(val));
                    return 16;
                // RLC A
                case 0x07:
                    val = get_register(A);
                    set_register(A, RLC(val));
                    return 8;
                // RLC B
                case 0x00:
                    val = get_register(B);
                    set_register(B, RLC(val));
                    return 8;
                // RLC C
                case 0x01:
                    val = get_register(C);
                    set_register(C, RLC(val));
                    return 8;
                // RLC D
                case 0x02:
                    val = get_register(D);
                    set_register(D, RLC(val));
                    return 8;
                // RLC E
                case 0x03:
                    val = get_register(E);
                    set_register(E, RLC(val));
                    return 8;
                // RLC H
                case 0x04:
                    val = get_register(H);
                    set_register(H, RLC(val));
                    return 8;
                // RLC L
                case 0x05:
                    val = get_register(L);
                    set_register(L, RLC(val))
                    return 8;
                // RLC <HL>
                case 0x06:
                    addr = get_16b_register(H, L);
                    val = read(addr);
                    write(addr, RLC(val));
                    return 16;
                // RL A
                case 0x17:
                    val = get_register(A);
                    set_register(A, RL(val));
                    return 8;
                // RL B
                case 0x10:
                    val = get_register(B);
                    set_register(B, RL(val));
                    return 8;
                // RL C
                case 0x11:
                    val = get_register(C);
                    set_register(C, RL(val));
                    return 8;
                // RL D
                case 0x12:
                    val = get_register(D);
                    set_register(D, RL(val));
                    return 8;
                // RL E
                case 0x13:
                    val = get_register(E);
                    set_register(E, RL(val));
                    return 8;
                // RL H
                case 0x14:
                    val = get_register(H);
                    set_register(H, RL(val));
                    return 8;
                // RL L
                case 0x15:
                    val = get_register(L);
                    set_register(L, RL(val));
                    return 8;
                // RL <HL>
                case 0x16:
                    addr = get_16b_register(H, L);
                    val = read(addr);
                    write(addr, RL(val));
                    return 16;
                // RRC A
                case 0x0F:
                    val = get_register(A);
                    set_register(A, RRC(val));
                    return 8;
                // RRC B
                case 0x08:
                    val = get_register(B);
                    set_register(B, RRC(val));
                    return 8;
                // RRC C
                case 0x09:
                    val = get_register(C);
                    set_register(C, RRC(val));
                    return 8;
                // RRC D
                case 0x0A:
                    val = get_register(D);
                    set_register(D, RRC(val));
                    return 8;
                // RRC E
                case 0x0B:
                    val = get_register(E);
                    set_register(E, RRC(val));
                    return 8;
                // RRC H
                case 0x0C:
                    val = get_register(H);
                    set_register(H, RRC(val));
                    return 8;
                // RRC L
                case 0x0D:
                    val = get_register(L);
                    set_register(L, RRC(val));
                    return 8;
                // RRC <HL>
                case 0x0E:
                    addr = get_16b_register(A, B);
                    val = read(addr);
                    write(addr, RRC(val));
                    return 16;
                // RR A
                case 0x1F:
                    val = get_register(A);
                    set_register(A, RR(val));
                    return 8;
                // RR B
                case 0x18:
                    val = get_register(B);
                    set_register(B, RR(val));
                    return 8;
                // RR C
                case 0x19:
                    val = get_register(C);
                    set_register(C, RR(val));
                    return 8;
                // RR D
                case 0x1A:
                    val = get_register(D);
                    set_register(D, RR(val));
                    return 8;
                // RR E
                case 0x1B:
                    val = get_register(E);
                    set_register(E, RR(val));
                    return 8;
                // RR H
                case 0x1C:
                    val = get_register(H);
                    set_register(H, RR(val));
                    return 8;
                // RR L
                case 0x1D:
                    val = get_register(L);
                    set_register(L, RR(val));
                    return 8;
                // RR <HL>
                case 0x1E:
                    addr = get_16b_register(H, L);
                    val = read(addr);
                    write(addr, RR(val));
                    return 16;
                // SLA A
                case 0x27:
                    val = get_register(A);
                    set_register(A, SLA(val));
                    return 8;
                // SLA B
                case 0x20:
                    val = get_register(B);
                    set_register(B, SLA(val));
                    return 8;
                // SLA C
                case 0x21:
                    val = get_register(C);
                    set_register(C, SLA(val));
                    return 8;
                // SLA D
                case 0x22:
                    val = get_register(D);
                    set_register(D, SLA(val));
                    return 8;
                // SLA E
                case 0x23:
                    val = get_register(E);
                    set_register(E, SLA(val));
                    return 8;
                // SLA H
                case 0x24:
                    val = get_register(H);
                    set_register(H, SLA(val));
                    return 8;
                // SLA L
                case 0x25:
                    val = get_register(L);
                    set_register(L, SLA(val));
                    return 8;
                // SLA <HL>
                case 0x26:
                    addr = get_16b_register(H, L);
                    val = read(addr);
                    write(addr, SLA(val));
                    return 16;
                // SRA A
                case 0x2F:
                    val = get_register(A);
                    set_register(A, SRA(val));
                    return 8;
                // SRA B
                case 0x2F:
                    val = get_register(B);
                    set_register(B, SRA(val));
                    return 8;
                // SRA C
                case 0x2F:
                    val = get_register(C);
                    set_register(C, SRA(val));
                    return 8;
                // SRA D
                case 0x2F:
                    val = get_register(D);
                    set_register(D, SRA(val));
                    return 8;
                // SRA E
                case 0x2F:
                    val = get_register(E);
                    set_register(E, SRA(val));
                    return 8;
                // SRA H
                case 0x2F:
                    val = get_register(H);
                    set_register(H, SRA(val));
                    return 8;
                // SRA L
                case 0x2F:
                    val = get_register(L);
                    set_register(L, SRA(val));
                    return 8;
                // SRA <HL>
                case 0x2F:
                    addr = get_16b_register(H, L);
                    val = read(addr);
                    write(addr, SRA(val));
                    return 16;
                // SRL A
                case 0x3F:
                    val = get_register(A);
                    write_register(A, SRL(val));
                    return 8;
                // SRL B
                case 0x38:
                    val = get_register(B);
                    write_register(B, SRL(val));
                    return 8;
                // SRL C
                case 0x39:
                    val = get_register(C);
                    write_register(C, SRL(val));
                    return 8;
                // SRL D
                case 0x3A:
                    val = get_register(D);
                    write_register(D, SRL(val));
                    return 8;
                // SRL E
                case 0x3B:
                    val = get_register(E);
                    write_register(E, SRL(val));
                    return 8;
                // SRL H
                case 0x3C:
                    val = get_register(H);
                    write_register(H, SRL(val));
                    return 8;
                // SRL L
                case 0x3D:
                    val = get_register(L);
                    write_register(L, SRL(val));
                    return 8;
                // SRL <HL>
                case 0x3E:
                    addr = get_16b_register(H, L);
                    val = read(addr);
                    write(addr, SRL(val));
                    return 16;
                case 0x47: BIT(get_register(A), 0); return 8; 
                case 0x4F: BIT(get_register(A), 1); return 8; 
                case 0x57: BIT(get_register(A), 2); return 8; 
                case 0x5F: BIT(get_register(A), 3); return 8; 
                case 0x67: BIT(get_register(A), 4); return 8; 
                case 0x6F: BIT(get_register(A), 5); return 8; 
                case 0x77: BIT(get_register(A), 6); return 8; 
                case 0x7F: BIT(get_register(A), 7); return 8; 
                case 0x40: BIT(get_register(B), 0); return 8; 
                case 0x48: BIT(get_register(B), 1); return 8; 
                case 0x50: BIT(get_register(B), 2); return 8; 
                case 0x58: BIT(get_register(B), 3); return 8; 
                case 0x60: BIT(get_register(B), 4); return 8; 
                case 0x48: BIT(get_register(B), 1); return 8;
                case 0x50: BIT(get_register(B), 2); return 8;
                case 0x58: BIT(get_register(B), 3); return 8;
                case 0x60: BIT(get_register(B), 4); return 8;
                case 0x68: BIT(get_register(B), 5); return 8;
                case 0x70: BIT(get_register(B), 6); return 8;
                case 0x78: BIT(get_register(B), 7); return 8;
                case 0x41: BIT(get_register(C), 0); return 8;
                case 0x49: BIT(get_register(C), 1); return 8;
                case 0x51: BIT(get_register(C), 2); return 8;
                case 0x59: BIT(get_register(C), 3); return 8;
                case 0x61: BIT(get_register(C), 4); return 8;
                case 0x69: BIT(get_register(C), 5); return 8;
                case 0x71: BIT(get_register(C), 6); return 8;
                case 0x79: BIT(get_register(C), 7); return 8;
                case 0x42: BIT(get_register(D), 0); return 8;
                case 0x4A: BIT(get_register(D), 1); return 8;
                case 0x52: BIT(get_register(D), 2); return 8;
                case 0x5A: BIT(get_register(D), 3); return 8;
                case 0x62: BIT(get_register(D), 4); return 8;
                case 0x6A: BIT(get_register(D), 5); return 8;
                case 0x72: BIT(get_register(D), 6); return 8;
                case 0x7A: BIT(get_register(D), 7); return 8;
                case 0x43: BIT(get_register(E), 0); return 8;
                case 0x4B: BIT(get_register(E), 1); return 8;
                case 0x53: BIT(get_register(E), 2); return 8;
                case 0x5B: BIT(get_register(E), 3); return 8;
                case 0x63: BIT(get_register(E), 4); return 8;
                case 0x6B: BIT(get_register(E), 5); return 8;
                case 0x73: BIT(get_register(E), 6); return 8;
                case 0x7B: BIT(get_register(E), 7); return 8;
                case 0x44: BIT(get_register(H), 0); return 8;
                case 0x4C: BIT(get_register(H), 1); return 8;
                case 0x54: BIT(get_register(H), 2); return 8;
                case 0x5C: BIT(get_register(H), 3); return 8;
                case 0x64: BIT(get_register(H), 4); return 8;
                case 0x6C: BIT(get_register(H), 5); return 8;
                case 0x74: BIT(get_register(H), 6); return 8;
		        case 0x7C: BIT(get_register(H), 7); return 8;
                case 0x45: BIT(get_register(L), 0); return 8;
                case 0x4D: BIT(get_register(L), 1); return 8;
                case 0x55: BIT(get_register(L), 2); return 8;
                case 0x5D: BIT(get_register(L), 3); return 8;
                case 0x65: BIT(get_register(L), 4); return 8;
                case 0x6D: BIT(get_register(L), 5); return 8;
                case 0x75: BIT(get_register(L), 6); return 8;
                case 0x7D: BIT(get_register(L), 7); return 8;
                case 0x46: BIT(read(get_16b_register(H, L)), 0); return 16;
                case 0x4E: BIT(read(get_16b_register(H, L)), 1); return 16;
                case 0x56: BIT(read(get_16b_register(H, L)), 2); return 16;
                case 0x5E: BIT(read(get_16b_register(H, L)), 3); return 16;
                case 0x66: BIT(read(get_16b_register(H, L)), 4); return 16;
                case 0x6E: BIT(read(get_16b_register(H, L)), 5); return 16;
                case 0x76: BIT(read(get_16b_register(H, L)), 6); return 16;
                case 0x7E: BIT(read(get_16b_register(H, L)), 7); return 16;

                case 0xC7: SET(A, 0); return 8;
                case 0xCF: SET(A, 1); return 8;
                case 0xD7: SET(A, 2); return 8;
                case 0xDF: SET(A, 3); return 8;
                case 0xE7: SET(A, 4); return 8;
                case 0xEF: SET(A, 5); return 8;
                case 0xF7: SET(A, 6); return 8;
                case 0xFF: SET(A, 7); return 8;
                case 0xC0: SET(B, 0); return 8;
                case 0xC8: SET(B, 1); return 8;
                case 0xD0: SET(B, 2); return 8;
                case 0xD8: SET(B, 3); return 8;
                case 0xE0: SET(B, 4); return 8;
                case 0xE8: SET(B, 5); return 8;
                case 0xF0: SET(B, 6); return 8;
                case 0xF8: SET(B, 7); return 8;
                case 0xC1: SET(C, 0); return 8;
                case 0xC9: SET(C, 1); return 8;
                case 0xD1: SET(C, 2); return 8;
                case 0xD9: SET(C, 3); return 8;
                case 0xE1: SET(C, 4); return 8;
                case 0xE9: SET(C, 5); return 8;
                case 0xF1: SET(C, 6); return 8;
                case 0xF9: SET(C, 7); return 8;
                case 0xC2: SET(D, 0); return 8;
                case 0xCA: SET(D, 1); return 8;
                case 0xD2: SET(D, 2); return 8;
                case 0xDA: SET(D, 3); return 8;
                case 0xE2: SET(D, 4); return 8;
                case 0xEA: SET(D, 5); return 8;
                case 0xF2: SET(D, 6); return 8;
                case 0xFA: SET(D, 7); return 8;
                case 0xC3: SET(E, 0); return 8;
                case 0xCB: SET(E, 1); return 8;
                case 0xD3: SET(E, 2); return 8;
                case 0xDB: SET(E, 3); return 8;
                case 0xE3: SET(E, 4); return 8;
                case 0xEB: SET(E, 5); return 8;
                case 0xF3: SET(E, 6); return 8;
                case 0xFB: SET(E, 7); return 8;
                case 0xC4: SET(H, 0); return 8;
                case 0xCC: SET(H, 1); return 8;
                case 0xD4: SET(H, 2); return 8;
                case 0xDC: SET(H, 3); return 8;
                case 0xE4: SET(H, 4); return 8;
                case 0xEC: SET(H, 5); return 8;
                case 0xF4: SET(H, 6); return 8;
                case 0xFC: SET(H, 7); return 8;
                case 0xC5: SET(L, 0); return 8;
                case 0xCD: SET(L, 1); return 8;
                case 0xD5: SET(L, 2); return 8;
                case 0xDD: SET(L, 3); return 8;
                case 0xE5: SET(L, 4); return 8;
                case 0xED: SET(L, 5); return 8;
                case 0xF5: SET(L, 6); return 8;
                case 0xFD: SET(L, 7); return 8;
                case 0xC6: SET_16B(H, L, 0); return 16;
                case 0xCE: SET_16B(H, L, 1); return 16;
                case 0xD6: SET_16B(H, L, 2); return 16;
                case 0xDE: SET_16B(H, L, 3); return 16;
                case 0xE6: SET_16B(H, L, 4); return 16;
                case 0xEE: SET_16B(H, L, 5); return 16;
                case 0xF6: SET_16B(H, L, 6); return 16;
                case 0xFE: SET_16B(H, L, 7); return 16;

                // case 0x87: RES(A, 0); op(2, 2); break;
                // case 0x8F: RES(A, 1); op(2, 2); break;
                // case 0x97: RES(A, 2); op(2, 2); break;
                // case 0x9F: RES(A, 3); op(2, 2); break;
                // case 0xA7: RES(A, 4); op(2, 2); break;
                // case 0xAF: RES(A, 5); op(2, 2); break;
                // case 0xB7: RES(A, 6); op(2, 2); break;
                // case 0xBF: RES(A, 7); op(2, 2); break;
                // case 0x80: RES(B, 0); op(2, 2); break;
                // case 0x88: RES(B, 1); op(2, 2); break;
                // case 0x90: RES(B, 2); op(2, 2); break;
                // case 0x98: RES(B, 3); op(2, 2); break;
                // case 0xA0: RES(B, 4); op(2, 2); break;
                // case 0xA8: RES(B, 5); op(2, 2); break;
                // case 0xB0: RES(B, 6); op(2, 2); break;
                // case 0xB8: RES(B, 7); op(2, 2); break;
                // case 0x81: RES(C, 0); op(2, 2); break;
                // case 0x89: RES(C, 1); op(2, 2); break;
                // case 0x91: RES(C, 2); op(2, 2); break;
                // case 0x99: RES(C, 3); op(2, 2); break;
                // case 0xA1: RES(C, 4); op(2, 2); break;
                // case 0xA9: RES(C, 5); op(2, 2); break;
                // case 0xB1: RES(C, 6); op(2, 2); break;
                // case 0xB9: RES(C, 7); op(2, 2); break;
                // case 0x82: RES(D, 0); op(2, 2); break;
                // case 0x8A: RES(D, 1); op(2, 2); break;
                // case 0x92: RES(D, 2); op(2, 2); break;
                // case 0x9A: RES(D, 3); op(2, 2); break;
                // case 0xA2: RES(D, 4); op(2, 2); break;
                // case 0xAA: RES(D, 5); op(2, 2); break;
                // case 0xB2: RES(D, 6); op(2, 2); break;
                // case 0xBA: RES(D, 7); op(2, 2); break;
                // case 0x83: RES(E, 0); op(2, 2); break;
                // case 0x8B: RES(E, 1); op(2, 2); break;
                // case 0x93: RES(E, 2); op(2, 2); break;
                // case 0x9B: RES(E, 3); op(2, 2); break;
                // case 0xA3: RES(E, 4); op(2, 2); break;
                // case 0xAB: RES(E, 5); op(2, 2); break;
                // case 0xB3: RES(E, 6); op(2, 2); break;
                // case 0xBB: RES(E, 7); op(2, 2); break;
                // case 0x84: RES(H, 0); op(2, 2); break;
                // case 0x8C: RES(H, 1); op(2, 2); break;
                // case 0x94: RES(H, 2); op(2, 2); break;
                // case 0x9C: RES(H, 3); op(2, 2); break;
                // case 0xA4: RES(H, 4); op(2, 2); break;
                // case 0xAC: RES(H, 5); op(2, 2); break;
                // case 0xB4: RES(H, 6); op(2, 2); break;
                // case 0xBC: RES(H, 7); op(2, 2); break;
                // case 0x85: RES(L, 0); op(2, 2); break;
                // case 0x8D: RES(L, 1); op(2, 2); break;
                // case 0x95: RES(L, 2); op(2, 2); break;
                // case 0x9D: RES(L, 3); op(2, 2); break;
                // case 0xA5: RES(L, 4); op(2, 2); break;
                // case 0xAD: RES(L, 5); op(2, 2); break;
                // case 0xB5: RES(L, 6); op(2, 2); break;
                // case 0xBD: RES(L, 7); op(2, 2); break;
                // case 0x86: RES(Pair(reg_H, reg_L).address(), 0); op(2, 4); break;
                // case 0x8E: RES(Pair(reg_H, reg_L).address(), 1); op(2, 4); break;
                // case 0x96: RES(Pair(reg_H, reg_L).address(), 2); op(2, 4); break;
                // case 0x9E: RES(Pair(reg_H, reg_L).address(), 3); op(2, 4); break;
                // case 0xA6: RES(Pair(reg_H, reg_L).address(), 4); op(2, 4); break;
                // case 0xAE: RES(Pair(reg_H, reg_L).address(), 5); op(2, 4); break;
                // case 0xB6: RES(Pair(reg_H, reg_L).address(), 6); op(2, 4); break;
                // case 0xBE: RES(Pair(reg_H, reg_L).address(), 7); op(2, 4); break;                   return 1;
                default:
                    printf("Unknown CB extended Instruction\n");
            }
            break;
    }
}
