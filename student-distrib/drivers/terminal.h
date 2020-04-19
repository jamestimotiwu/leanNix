#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "../types.h"

#define VIDEO       0xB8000
#define NUM_COLS    80
#define NUM_ROWS    25
#define ATTRIB      0x7

#define TAB_STOP 4

/* basic terminal functionality */
void term_clear();
void term_showbuf(); /* prints out contents currently in buffer */
void term_putc(uint8_t c);
void term_test_int();

/* Functions used by keyboard */
int term_keyboardChar(uint8_t c);
void term_keyboardBackspace();
void term_keyboardEnter();
void term_keyboardTab();
void term_setChar(uint8_t c);

/* system calls */
int32_t terminal_open(const uint8_t *fname);
int32_t terminal_close(int32_t fd);
int32_t terminal_read(int32_t fd, void *buf, uint32_t count);
int32_t terminal_write(int32_t fd, void *buf, uint32_t count);

#endif
