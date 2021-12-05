#include "utils.h"
#include <stdbool.h>

int execute_next();
int execute_extended_instruction(BYTE prefix, BYTE op);
BYTE add_with_flags_u8(BYTE a, BYTE b, bool add_carry, bool affect_carry);
WORD add_with_flags_u16(WORD a, WORD b);
BYTE sub_with_flags_u8(BYTE a, BYTE b, bool sub_carry, bool affect_carry);

