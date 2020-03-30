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

/* Functions used by keyboard */
void term_keyboardChar(uint8_t c);
void term_keyboardBackspace();
void term_keyboardEnter();
void term_keyboardTab();
void term_setChar(uint8_t c);

#endif
