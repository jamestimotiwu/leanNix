#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../x86_desc.h"
#include "../lib.h"
#include "../i8259.h"

#define KB_IRQ     0x1
#define KB_DATA    0x60
#define KB_COMMAND 0x64
void keyboard_int();
void keyboard_init();
#endif
