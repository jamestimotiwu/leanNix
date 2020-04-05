#ifndef _SYSCALL_H
#define _SYSCALL_H

#define SYS_HALT 1
#define SYS_EXECUTE 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6

#define NUM_SYSCALLS 6

#ifndef ASM
/* the c code for the syscall handler */
void syscall_int();
/* syscall handler asm code */
void syscall_assembly();
#endif


#endif
