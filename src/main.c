#include <stdio.h>
#include "../include/cpu.h"
#include "../include/mmu.h"

#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 160

int main() {
    // // Read rom files
    // FILE *boot_rom = fopen("roms/boot.gb", "r");
    // FILE *game_rom = fopen("roms/rom1.gb", "r");

    // if(boot_rom == NULL || game_rom == NULL) {
    //     fprintf(stderr, "Could not open all required files\n");
    //     return 1;
    // }
    // // fread(mem, 1, 0x100, boot_rom);

    // fclose(boot_rom);
    // fclose(game_rom);

    // // Execute the program
    // execute_next();
    printf("%02X\n", get_register(A));
    set_register(A, 2);
    printf("%02X\n", get_register(A));
    printf("%04X\n", get_16b_register(A, B));
    set_16b_register(A, B, 2);
    printf("%04X\n", get_16b_register(A, B));
    return 0;
}
