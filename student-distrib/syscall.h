#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"
/* mask 8MB - 8KB*/
#define PCB_MASK 0x7FE000

/* the c code for the syscall handler */
void syscall_int();
/* syscall handler asm code */
void syscall_assembly();

PCB_t* get_PCB();
#endif
