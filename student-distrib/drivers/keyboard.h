#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../x86_desc.h"
#include "../lib.h"
#include "../i8259.h"
#include "../types.h"

#define KB_IRQ     0x1
#define KB_DATA    0x60
#define KB_COMMAND 0x64
#define KB_BUF_SIZE 128
#define KB_STAT    4 

// scancoodes in hex 
// reference used : https://wiki.osdev.org/PS2_Keyboard
//shifts, ctrl, and alt needs to be distinguished between pressed/released state

#define TAB  	   0x0F
#define CAPS_LOCK  0x3A
#define SHIFT_L_P  0x2A  
#define SHIFT_L_R  0xAA
#define SHIFT_R_P  0x36
#define SHIFT_R_R  0xB6 
#define CTRL_P     0x1D 
#define CTRL_R     0x9D
#define BACK_SPACE 0x0E
#define ENTER	   0x1C
#define ALT_P 	   0x38
#define ALT_R	   0xB8
#define CHAR_L     0x26
#define CHAR_TAB   '\t'
#define NULL_BYTE  '\0'
#define NOT_PRINT  '\0'
#define NEW_LINE   '\n'
#define char_space ' '
#define SPACE      0x39

//define pressed/released 

#define Pressed 1
#define Released 0

/* These are used by terminal driver but are located in keyboard.c */
extern volatile int cur_buf_length;
extern volatile char kb_buf[KB_BUF_SIZE];

/* keyboard functions */
extern void keyboard_int();
extern void keyboard_init();
char get_char_map(uint8_t sc); 
void reset_kb_buf();
void reset_cursor(int x, int y);
void tab_func();
void backsp_func();
void enter_func();
#endif


