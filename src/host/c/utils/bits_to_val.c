#include "../global_vars/globals.h"

uint8_t bits2val(char *bits) /// https://stackoverflow.com/questions/11310796/binary-to-unsigned-int-using-bitwise-operations-and-pointer-arithmetic-in-c
{
     uint8_t val;

    for (val = 0; *bits; bits++) {
        if (*bits == '1') 
            val = (val << 1) | 1;
        else if (*bits == '0' ) 
            val <<= 1;
        else 
            break;
    }

    return val;
}
