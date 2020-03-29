#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "../types.h"

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define ATTRIB      0x7


void term_clear();
void term_putc(uint8_t c);
void term_test_int();

#endif
