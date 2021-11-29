#include <stdio.h>
#include "../include/mmu.h"

extern BYTE mem[];
extern BYTE registers[];
extern WORD pc, sp;

BYTE add_with_flags_u8(BYTE a, BYTE b, bool add_carry, bool affect_carry) {
    // Reset all the flags
    set_register(F, 0);

    if(add_carry) {
        if(get_flag(FLAG_C)) {
            b++:
        }
    }

    BYTE res = a + b;

    // Set the carry flag accordingly
    if(affect_carry) {
        set_flag_to(C, res < a);
    }

    // Calculate the half carry flag
    set_flag_to(H, (a & 0xF) + (b & 0xF) > 0xF);

    return res;
}

WORD add_with_flags_u16(WORD a, WORD b) {
    WORD res = a + b;

    // Check for an overflow
    if(res < a) {
        set_flag(FLAG_C);
    }

    unset_flag(FLAG_N);

    // Calculate the half carry flag
    set_flag_to(H, (a & 0xFFF) + (b & 0xFFF) > 0xFFF);

    return res;
}

BYTE sub_with_flags_u8(BYTE a, BYTE b, bool sub_carry, bool affect_carry) {
    // Reset all the flags
    registers[F] = 0;
    set_flag(FLAG_N);

    // Set the carry flag accordingly
    if(sub_carry) {
        if(get_flag(FLAG_C)) {
            b++;
        }
    }

    BYTE res = a - b;

    if(affect_carry) {
        set_flag_to(C, res > a);
    }

    // Calculate the half carry flag
    SIGNED_BYTE a_ = a & 0xF;
    set_flag_to(H, a_ - (b & 0xF) < 0);

    return res;
}

// Execute the next instruction, increment the program counter and return the 
// number of simulated clock cycles 
int execute_next() {
    BYTE val;
    WORD addr;
    switch(read_next()) {
        // LD B, n
        case 0x06:
            set_register(B, read_next());
            return 8;
        // LD C, n
        case 0x0E:
            set_register(C, read_next());
            return 8;
        // LD D, n
        case 0x16:
            set_register(D, read_next());
            return 8;
        // LD E, n
        case 0x1E:
            set_register(E, read_next());
            return 8;
        // LD H, n
        case 0x26:
            set_register(H, read_next());
            return 8;
        // LD L, n
        case 0x2E:
            set_register(L, read_next());
            return 8;
        // LD A, A
        case 0x7F:
            set_register(A, get_register(A));
            return 4;
        // LD A, B
        case 0x78:
            set_register(A, get_register(B));
            return 4;
        // LD A, C
        case 0x79:
            set_register(A, get_register(C));
            return 4;
        // LD A, D
        case 0x7A:
            set_register(A, get_register(D));
            return 4;
        // LD A, E
        case 0x7B:
            set_register(A, get_register(E));
            return 4;
        // LD A, H
        case 0x7C:
            set_register(A, get_register(H));
            return 4;
        // LD A, L
        case 0x7D:
            set_register(A, get_register(L));
            return 4;
        // LD A, <BC>
        case 0x0A:
            addr = get_16b_register(B, C);
            set_register(A, read(addr));
            return 8;
        // LD A, <DE>
        case 0x1A:
            addr = get_16b_register(D, E);
            set_register(A, read(addr));
            return 8;
        // LD A, <HL>
        case 0x7E:
            addr = get_16b_register(H, L);
            set_register(A, read(addr));
            return 8;
        // LD A, <nn>
        case 0xFA:
            addr = bytes_to_word(read_next(), read_next());
            set_register(A, read(addr));
            return 16;
        // LD A, #
        case 0x3E:
            set_register(A, read_next());
            return 8;
        // LD B, A
        case 0x47:
            set_register(B, get_register(A));
            return 4;
        // LD B, B
        case 0x40:
            set_register(B, get_register(B));
            return 4;
        // LD B, C
        case 0x41:
            set_register(B, get_register(C));
            return 4;
        // LD B, D
        case 0x42:
            set_register(B, get_register(D));
            return 4;
        // LD B, E
        case 0x43:
            set_register(B, get_register(E));
            return 4;
        // LD B, H
        case 0x44:
            set_register(B, get_register(H));
            return 4;
        // LD B, L
        case 0x45:
            set_register(B, get_register(L));
            return 4;
        // LD B, <HL>
        case 0x46:
            addr = get_16b_register(H, L);
            set_register(B, read(addr));
            return 8;
        // LD C, A
        case 0x4F:
            set_register(C, get_register(A));
            return 4;
        // LD C, B
        case 0x48:
            set_register(C, get_register(B));
            return 4;
        // LD C, C
        case 0x49:
            set_register(C, get_register(C));
            return 4;
        // LD C, D
        case 0x4A:
            set_register(C, get_register(D));
            return 4;
        // LD C, E
        case 0x4B:
            set_register(C, get_register(E));
            return 4;
        // LD C, H
        case 0x4C:
            set_register(C, get_register(H));
            return 4;
        // LD C, L
        case 0x4D:
            set_register(C, get_register(L));
            return 4;
        // LD C, <HL>
        case 0x4E:
            addr = get_16b_register(H, L);
            set_register(C, read(addr));
            return 8;
        // LD D, A
        case 0x57:
            set_register(D, get_register(A));
            return 4;
        // LD D, B
        case 0x50:
            set_register(D, get_register(B));
            return 4;
        // LD D, C
        case 0x51:
            set_register(D, get_register(C));
            return 4;
        // LD D, D
        case 0x52:
            set_register(D, get_register(D));
            return 4;
        // LD D, E
        case 0x53:
            set_register(D, get_register(E));
            return 4;
        // LD D, H
        case 0x54:
            set_register(D, get_register(H));
            return 4;
        // LD D, L
        case 0x55:
            set_register(D, get_register(L));
            return 4;
        // LD D, <HL>
        case 0x56:
            addr = get_16b_register(H, L);
            set_register(D, read(addr));
            return 8;
        // LD E, A
        case 0x5F:
            set_register(E, get_register(A));
            return 4;
        // LD E, B
        case 0x58:
            set_register(E, get_register(B));
            return 4;
        // LD E, C
        case 0x59:
            set_register(E, get_register(C));
            return 4;
        // LD E, D
        case 0x5A:
            set_register(E, get_register(D));
            return 4;
        // LD E, E
        case 0x5B:
            set_register(E, get_register(E));
            return 4;
        // LD E, H
        case 0x5C:
            set_register(E, get_register(H));
            return 4;
        // LD E, L
        case 0x5D:
            set_register(E, get_register(L));
            return 4;
        // LD E, <HL>
        case 0x5E:
            addr = get_16b_register(H, L);
            set_register(E, read(addr));
            return 8;
        // LD H, A
        case 0x67:
            set_register(H, get_register(A));
            return 4;
        // LD H, B
        case 0x60:
            set_register(H, get_register(B));
            return 4;
        // LD H, C
        case 0x61:
            set_register(H, get_register(C));
            return 4;
        // LD H, D
        case 0x62:
            set_register(H, get_register(D));
            return 4;
        // LD H, E
        case 0x63:
            set_register(H, get_register(E));
            return 4;
        // LD H, H
        case 0x64:
            set_register(H, get_register(H));
            return 4;
        // LD H, L
        case 0x65:
            set_register(H, get_register(L));
            return 4;
        // LD H, <HL>
        case 0x66:
            addr = get_16b_register(H, L);
            set_register(H, read(addr));
            return 8;
        // LD L, A
        case 0x6F:
            set_register(L, get_register(A));
            return 4;
        // LD L, B
        case 0x68:
            set_register(L, get_register(B));
            return 4;
        // LD L, C
        case 0x69:
            set_register(L, get_register(C));
            return 4;
        // LD L, D
        case 0x6A:
            set_register(L, get_register(D));
            return 4;
        // LD L, E
        case 0x6B:
            set_register(L, get_register(E));
            return 4;
        // LD L, H
        case 0x6C:
            set_register(L, get_register(H));
            return 4;
        // LD L, L
        case 0x6D:
            set_register(L, get_register(L));
            return 4;
        // LD L, <HL>
        case 0x6E:
            addr = get_16b_register(H, L);
            set_register(L, read(addr));
            return 8;
        // LD <HL>, B
        case 0x70:
            addr = get_16b_register(H, L);
            write(addr, get_register(B));
            return 8;
        // LD <HL>, C
        case 0x71:
            addr = get_16b_register(H, L);
            write(addr, get_register(C));
            return 8;
        // LD <HL>, D
        case 0x72:
            addr = get_16b_register(H, L);
            write(addr, get_register(D));
            return 8;
        // LD <HL>, E
        case 0x73:
            addr = get_16b_register(H, L);
            write(addr, get_register(E));
            return 8;
        // LD <HL>, H
        case 0x74:
            addr = get_16b_register(H, L);
            write(addr, get_register(H));
            return 8;
        // LD <HL>, L
        case 0x75:
            addr = get_16b_register(H, L);
            write(addr, get_register(L));
            return 8;
        // LD <HL>, n
        case 0x36:
            addr = get_16b_register(H, L);
            write(addr, read_next());
            return 12;
        // LD <BC>, A
        case 0x02:
            addr = get_16b_register(B, C);
            write(addr, get_register(A));
            return 8;
        // LD <DE>, A
        case 0x12:
            addr = get_16b_register(D, E);
            write(addr, get_register(A));
            return 8;
        // LD <HL>, A
        case 0x77:
            addr = get_16b_register(H, L);
            write(addr, get_register(A));
            return 8;
        // LD <nn>, A
        case 0xEA:
            addr = bytes_to_word(read_next(), read_next());
            write(addr, get_register(A));
            return 16;
        // LD A, <0xFF00 + C>
        case 0xF2:
            addr = 0xFF00 + get_register(C);
            set_register(A, read(addr));
            return 8;
        // LD <0xFF00 + C>
        case 0xE2:
            addr = 0xFF00 + get_register(C);
            write(addr, get_register(A));
            return 8;
        // LDD A, <HL>
        case 0x3A:
            addr = get_16b_register(H, L);
            set_register(A, read(addr));
            addr--;
            set_16b_register(H, L, addr);
            return 8;
        // LDD <HL>, A
        case 0x32:
            addr = get_16b_register(H, L);
            write(addr, get_register(A));
            addr--;
            set_16b_register(H, L, addr);
            return 8;
        // LDI A, <HL>
        case 0x2A:
            addr = get_16b_register(H, L);
            set_register(A, read(addr));
            addr++;
            set_16b_register(H, L, addr);
            return 8;
        // LDI <HL>, A
        case 0x22:
            addr = get_16b_register(H, L);
            write(addr, get_register(A));
            addr++;
            set_16b_register(H, L, addr);
            return 8;
        // LDH <0xFF00 + n>, A
        case 0xE0:
            addr = 0xFF00 + read_next();
            write(addr, get_register(A));
            return 12;
        // LDH A, <0xFF00 + n>
        case 0xF0:
            addr = 0xFF00 + read_next();
            set_register(A, read(addr));
            return 12;
        // LD BC, nn
        case 0x01:
            set_16b_register(B, C, read_next_word());
            return 12;
        // LD DE, nn
        case 0x11:
            set_16b_register(D, E, read_next_word());
            return 12;
        // LD HL, nn
        case 0x21:
            set_16b_register(H, L, read_next_word());
            return 12;
        // LD SP, nn
        case 0x31:
            sp = read_next_word();
            return 12;
        // LD SP, HL
        case 0xF9:
            sp = get_16b_register(H, L);
            return 8;
        // LDHL SP, n
        case 0xF8:
            val = read_next();
            // Hacky was to set the flags as if it was an 8bit operation
            add_with_flags_8b((BYTE)sp, val_2, false, true);

            set_16b_register(H, L, sp + val);
            // This instruction resets the zero flag
            unset_flag(FLAG_Z);
            return 12;
        // LD <nn>, SP
        case 0x08:
            addr = read_next_word();
            write(addr, sp >> 8);
            write(addr + 1, sp & 0xFF);
            return 20;
        // PUSH AF
        case 0xF5:
            write_word(sp, get_16b_register(A, F));
            sp -= 2;
            return 16;
        // PUSH BC
        case 0xC5:
            write_word(sp, get_16b_register(B, C));
            sp -= 2;
            return 16;
        // PUSH DE
        case 0xD5:
            write_word(sp, get_16b_register(D, E));
            sp -= 2;
            return 16;
        // PUSH HL
        case 0xE5:
            write_word(sp, get_16b_register(H, L));
            sp -= 2;
            return 16;
        // POP AF
        case 0xF1:
            set_16b_register(A, F, read_word(sp));
            sp += 2;
            return 12;
        // POP BC
        case 0xC1:
            set_16b_register(B, C, read_word(sp));
            sp += 2;
            return 12;
        // POP DE
        case 0xD1:
            set_16b_register(D, E, read_word(sp));
            sp += 2;
            return 12;
        // POP HL
        case 0xE1:
            set_16b_register(H, L, read_word(sp));
            sp += 2;
            return 12;
        // ADD A, A
        case 0x87:
            val = add_with_flags_u8(get_register(A), get_register(A), false, true);
            set_register(A, val);
            return 4;
        // ADD A, B
        case 0x80:
            val = add_with_flags_u8(get_register(A), get_register(B), false, true);
            set_register(A, val);
            return 4;
        // ADD A, C
        case 0x81:
            val = add_with_flags_u8(get_register(A), get_register(C), false, true);
            set_register(A, val);
            return 4;
        // ADD A, D
        case 0x82:
            val = add_with_flags_u8(get_register(A), get_register(D), false, true);
            set_register(A, val);
            return 4;
        // ADD A, E
        case 0x83:
            val = add_with_flags_u8(get_register(A), get_register(E), false, true);
            set_register(A, val);
            return 4;
        // ADD A, H
        case 0x84:
            val = add_with_flags_u8(get_register(A), get_register(H), false, true);
            set_register(A, val);
            return 4;
        // ADD A, L
        case 0x85:
            val = add_with_flags_u8(get_register(A), get_register(L), false, true);
            set_register(A, val);
            return 4;
        // ADD A, <HL>
        case 0x86:
            addr = get_16b_register(H, L);
            val = add_with_flags_u8(get_register(A), read(addr), false, true);
            set_register(A, val);
            return 8;
        // ADD A, #
        case 0xC6:
            val = add_with_flags_u8(get_register(A), read_next(), false, true);
            set_register(A, val);
            return 8;
        // ADC A, A
        case 0x8F:
            val = add_with_flags_u8(get_register(A), get_register(A), true, true);
            set_register(A, val);
            return 4;
        // ADC A, B
        case 0x88:
            val = add_with_flags_u8(get_register(A), get_register(B), true, true);
            set_register(A, val);
            return 4;
        // ADC A, C
        case 0x89:
            val = add_with_flags_u8(get_register(A), get_register(C), true, true);
            set_register(A, val);
            return 4;
        // ADC A, D
        case 0x8A:
            val = add_with_flags_u8(get_register(A), get_register(D), true, true);
            set_register(A, val);
            return 4;
        // ADC A, E
        case 0x8B:
            val = add_with_flags_u8(get_register(A), get_register(E), true, true);
            set_register(A, val);
            return 4;
        // ADC A, H
        case 0x8C:
            val = add_with_flags_u8(get_register(A), get_register(H), true, true);
            set_register(A, val);
            return 4;
        // ADC A, L
        case 0x8D:
            val = add_with_flags_u8(get_register(A), get_register(L), true, true);
            set_register(A, val);
            return 4;
        // ADC A, <HL>
        case 0x8E:
            addr = get_16b_register(H, L);
            val = add_with_flags_u8(get_register(A), read(addr), true, true);
            set_register(A, val);
            return 8;
        // ADC A, #
        case 0xCE:
            val = add_with_flags_u8(get_register(A), read_next(), true, true);
            set_register(A, val);
            return 8;
        // SUB A, A
        case 0x97:
            val = sub_with_flags_u8(get_register(A), get_register(A), false, true);
            set_register(A, val);
            return 4;
        // SUB A, B
        case 0x90:
            val = sub_with_flags_u8(get_register(A), get_register(B), false, true);
            set_register(A, val);
            return 4;
        // SUB A, C
        case 0x91:
            val = sub_with_flags_u8(get_register(A), get_register(C), false, true);
            set_register(A, val);
            return 4;
        // SUB A, D
        case 0x92:
            val = sub_with_flags_u8(get_register(A), get_register(D), false, true);
            set_register(A, val);
            return 4;
        // SUB A, E
        case 0x93:
            val = sub_with_flags_u8(get_register(A), get_register(E), false, true);
            set_register(A, val);
            return 4;
        // SUB A, H
        case 0x94:
            val = sub_with_flags_u8(get_register(A), get_register(H), false, true);
            set_register(A, val);
            return 4;
        // SUB A, L
        case 0x95:
            val = sub_with_flags_u8(get_register(A), get_register(L), false, true);
            set_register(A, val);
            return 4;
        // SUB A, <HL>
        case 0x96:
            addr = get_16b_register(H, L);
            val = sub_with_flags_u8(get_register(A), read(addr), false, true);
            set_register(A, val);
            return 8;
        // SUB A, #
        case 0xD6:
            val = sub_with_flags_u8(get_register(A), read_next(), false, true);
            set_register(A, val);
            return 8;
        // SBC A, A
        case 0x9F:
            val = sub_with_flags_u8(get_register(A), get_register(A), true, true);
            set_register(A, val);
            return 4;
        // SBC A, B
        case 0x98:
            val = sub_with_flags_u8(get_register(A), get_register(B), true, true);
            set_register(A, val);
            return 4;
        // SBC A, C
        case 0x99:
            val = sub_with_flags_u8(get_register(A), get_register(C), true, true);
            set_register(A, val);
            return 4;
        // SBC A, D
        case 0x9A:
            val = sub_with_flags_u8(get_register(A), get_register(D), true, true);
            set_register(A, val);
            return 4;
        // SBC A, E
        case 0x9B:
            val = sub_with_flags_u8(get_register(A), get_register(E), true, true);
            set_register(A, val);
            return 4;
        // SBC A, H
        case 0x9C:
            val = sub_with_flags_u8(get_register(A), get_register(H), true, true);
            set_register(A, val);
            return 4;
        // SBC A, L
        case 0x9D:
            val = sub_with_flags_u8(get_register(A), get_register(L), true, true);
            set_register(A, val);
            return 4;
        // SBC A, <HL>
        case 0x9E:
            addr = get_16b_register(H, L);
            val = sub_with_flags_u8(get_register(A), read(addr), true, true);
            set_register(A, val);
            return 8;
        // // SBC A, #
        // Unknown Opcode. Probably never used anyways
        // case 0xCE:
        //     val = add_with_flags_u8(get_register(A), read_next(), true, true);
        //     set_register(A, val);
        //     return ??;
        // AND A, A
        case 0xA7:
            set_register(A, get_register(A) & get_register(A));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            set_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // AND A, B
        case 0xA0:
            set_register(A, get_register(A) & get_register(B));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            set_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // AND A, C
        case 0xA1:
            set_register(A, get_register(A) & get_register(C));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            set_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // AND A, D
        case 0xA2:
            set_register(A, get_register(A) & get_register(D));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            set_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // AND A, E
        case 0xA3:
            set_register(A, get_register(A) & get_register(E));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            set_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // AND A, H
        case 0xA4:
            set_register(A, get_register(A) & get_register(H));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            set_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // AND A, L
        case 0xA5:
            set_register(A, get_register(A) & get_register(L));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            set_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // AND A, <HL>
        case 0xA4:
            addr = get_16b_register(H, L);
            set_register(A, get_register(A) & read(addr));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            set_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 8;
        // AND A, #
        case 0xA4:
            set_register(A, get_register(A) & read_next());
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            set_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 8;
        // OR A, A
        case 0xB7:
            set_register(A, get_register(A) | get_register(A));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // OR A, B
        case 0xB0:
            set_register(A, get_register(A) | get_register(B));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // OR A, C
        case 0xB1:
            set_register(A, get_register(A) | get_register(C));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // OR A, D
        case 0xB2:
            set_register(A, get_register(A) | get_register(D));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // OR A, E
        case 0xB3:
            set_register(A, get_register(A) | get_register(E));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // OR A, H
        case 0xB4:
            set_register(A, get_register(A) | get_register(H));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // OR A, L
        case 0xB5:
            set_register(A, get_register(A) | get_register(L));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // OR A, <HL>
        case 0xB6:
            addr = get_16b_register(H, L);
            set_register(A, get_register(A) | read(addr));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 8;
        // OR A, #
        case 0xF6:
            set_register(A, get_register(A) | read_next());
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 8;
        // XOR A, A
        case 0xAF:
            set_register(A, get_register(A) ^ get_register(A));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // XOR A, B
        case 0xA8:
            set_register(A, get_register(A) ^ get_register(B));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // XOR A, C
        case 0xA9:
            set_register(A, get_register(A) ^ get_register(C));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // XOR A, D
        case 0xAA:
            set_register(A, get_register(A) ^ get_register(D));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // XOR A, E
        case 0xAB:
            set_register(A, get_register(A) ^ get_register(E));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // XOR A, H
        case 0xAC:
            set_register(A, get_register(A) ^ get_register(H));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // XOR A, L
        case 0xAD:
            set_register(A, get_register(A) ^ get_register(L));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 4;
        // XOR A, <HL>
        case 0xAE:
            addr = get_16b_register(H, L);
            set_register(A, get_register(A) ^ read(addr));
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 8;
        // XOR A, #
        case 0xEE:
            set_register(A, get_register(A) ^ read_next());
            set_flag_to(FLAG_Z, get_register(A) == 0);
            unset_flag(FLAG_N);
            unset_flag(FLAG_H);
            unset_flag(FLAG_C);
            return 8;
        // CP A, A
        case 0xBF:
            sub_with_flags_u8(get_register(A), get_register(A), false);
            return 4;
        // CP A, B
        case 0xB8:
            sub_with_flags_u8(get_register(A), get_register(B), false);
            return 4;
        // CP A, C
        case 0xB9:
            sub_with_flags_u8(get_register(A), get_register(C), false);
            return 4;
        // CP A, D
        case 0xBA:
            sub_with_flags_u8(get_register(A), get_register(D), false);
            return 4;
        // CP A, E
        case 0xBB:
            sub_with_flags_u8(get_register(A), get_register(E), false);
            return 4;
        // CP A, H
        case 0xBC:
            sub_with_flags_u8(get_register(A), get_register(H), false);
            return 4;
        // CP A, L
        case 0xBD:
            sub_with_flags_u8(get_register(A), get_register(L), false);
            return 4;
        // CP A, <HL>
        case 0xBE:
            addr = get_16b_register(H, L);
            sub_with_flags_u8(get_register(A), read(addr), false);
            return 8;
        // CP A, #
        case 0xFE:
            sub_with_flags_u8(get_register(A), read_next(), false);
            return 8;
        // INC A
        case 0x3C:
            val = add_with_flags_u8(get_register(A), 1, false, false);
            set_register(A, val);
        // INC B
        case 0x04:
            val = add_with_flags_u8(get_register(B), 1, false, false);
            set_register(B, val);
        // INC C
        case 0x0C:
            val = add_with_flags_u8(get_register(C), 1, false, false);
            set_register(C, val);
        // INC D
        case 0x14:
            val = add_with_flags_u8(get_register(D), 1, false, false);
            set_register(D, val);
        // INC E
        case 0x1C:
            val = add_with_flags_u8(get_register(E), 1, false, false);
            set_register(E, val);
        // INC H
        case 0x24:
            val = add_with_flags_u8(get_register(H), 1, false, false);
            set_register(H, val);
        // INC L
        case 0x2C:
            val = add_with_flags_u8(get_register(L), 1, false, false);
            set_register(L, val);
        // INC <HL>
        case 0x34:
            addr = get_16b_register(H, L);
            val = add_with_flags_u8(read(addr), 1, false, false);
            write(addr, val);
        // DEC A
        case 0x3D:
            val = sub_with_flags_u8(get_register(A), 1, false, false);
            set_register(A, val);
        // DEC B
        case 0x05:
            val = sub_with_flags_u8(get_register(B), 1, false, false);
            set_register(B, val);
        // DEC C
        case 0x0D:
            val = sub_with_flags_u8(get_register(C), 1, false, false);
            set_register(C, val);
        // DEC D
        case 0x15:
            val = sub_with_flags_u8(get_register(D), 1, false, false);
            set_register(D, val);
        // DEC E
        case 0x1D:
            val = sub_with_flags_u8(get_register(E), 1, false, false);
            set_register(E, val);
        // DEC H
        case 0x25:
            val = sub_with_flags_u8(get_register(H), 1, false, false);
            set_register(H, val);
        // DEC L
        case 0x2D:
            val = sub_with_flags_u8(get_register(L), 1, false, false);
            set_register(L, val);
        // DEC <HL>
        case 0x35:
            addr = get_16b_register(H, L);
            val = sub_with_flags_u8(read(addr), 1, false, false);
            write(addr, val);
        // ADD HL, BC
        case 0x09:
            val = add_with_flags_u16(
                    get_16b_register(H, L),
                    get_16b_register(B, C)
                    );
            set_16b_register(H, L, val);
            return 8;
        // ADD HL, DE
        case 0x19:
            val = add_with_flags_u16(
                    get_16b_register(H, L),
                    get_16b_register(D, E)
                    );
            set_16b_register(H, L, val);
            return 8;
        // ADD HL, HL
        case 0x29:
            val = add_with_flags_u16(
                    get_16b_register(H, L),
                    get_16b_register(H, L)
                    );
            set_16b_register(H, L, val);
            return 8;
        // ADD HL, SP
        case 0x39:
            val = add_with_flags_u16(
                    get_16b_register(H, L),
                    sp,
                    );
            set_16b_register(H, L, val);
            return 8;
        // ADD SP, #
        case 0xE8:
            // this instruction resets the zero flag
            unset_flag(FLAG_Z);
            return 16;


        default:
            printf("Unknown Instruction\n");
            return 1;
    }
}
