#ifndef _SYSCALL_H
#define _SYSCALL_H

#include "types.h"

/* the c code for the syscall handler */
void syscall_int();
/* syscall handler asm code */
void syscall_assembly();

PCB_t* get_PCB();
#endif
