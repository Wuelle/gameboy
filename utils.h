typedef unsigned char BYTE;
typedef short WORD;

WORD bytes_to_word(BYTE a, BYTE b) {
    // TODO: is this the right order?
    return ((WORD)a << 8) + b;
}
