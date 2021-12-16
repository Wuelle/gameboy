#include <stdbool.h> 
#include "utils.h"

#define MEM_SIZE 0x10000

BYTE read_next();
WORD read_next_word();
BYTE read(WORD addr);
WORD read_word(WORD addr);
void write(WORD addr, BYTE data);
void write_word(WORD addr, WORD data);
