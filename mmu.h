#include <stdbool.h> 
#include <utils.h>

#define MEM_SIZE 0x10000
#define NUM_REGISTERS 8
#define B 0
#define C 1
#define D 2
#define E 3
#define F 4
#define H 5
#define L 6
#define A 7
#define FLAG_C 4
#define FLAG_H 5
#define FLAG_N 6
#define FLAG_Z 7

BYTE read_next();
WORD read_next_word();
BYTE read(WORD addr);
void write(WORD addr, BYTE data);

BYTE get_register(BYTE ix);
void set_register(BYTE ix, BYTE val);

WORD get_16b_register(BYTE ix1, BYTE ix2);
void set_16b_register(char ix1, char ix2, WORD data);

bool get_flag(BYTE ix);
void set_flag(BYTE ix);
void unset_flag(BYTE ix);
