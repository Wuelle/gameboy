#include <stdio.h>
#include <mmu.h>

extern BYTE mem[];
extern WORD pc, sp;

// Execute the next instruction, increment the program counter and return the 
// number of simulated clock cycles 
int execute_next() {
    BYTE val = read(pc);
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
        // case 0xF8:

        default:
            printf("Unknown Instruction\n");
    }
    return 0;
}
