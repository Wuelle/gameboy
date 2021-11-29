# https://stackoverflow.com/questions/30573481/how-to-write-a-makefile-with-separate-source-and-header-directories
CC=gcc
CFLAGS=-B src
DEPS=gameboy.h

SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

# OBJ := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

gb:
	$(CC) -o $(BIN_DIR)/gameboy src/main.c src/cpu.c src/mmu.c src/utils.c $(CFLAGS)
test:
	$(CC) -o $(BIN_DIR)/gameboy_tests tests/flags.c $(CFLAGS)
clean: 
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)
