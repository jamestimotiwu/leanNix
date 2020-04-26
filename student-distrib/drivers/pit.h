#ifndef PIT_H
#define PIT_H

#include "../x86_desc.h"
#include "../lib.h"
#include "../i8259.h"
#include "../types.h"

#define PIT_IRQ 0
#define PIT_COMMAND 0x43
#define PIT_DATA_CHANNEL_0 0x40
#define PIT_COMMAND_BYTE 0x36
#define PIT_BASE 1193180
#define PIT_MASK 0xFF


/* PIT interrupt handler */
void pit_int();
/* PIT initializer */
void pit_init(uint32_t frequency);


#endif
