
#define ASM
#include "syscall.h"

/* wrapper functions for the system calls (called by C code) */
/* very similar to the code in ece391_syscall.S */

#define DO_CALL(name,number)   \
.GLOBL name                   ;\
name:   PUSHL	%EBX          ;\
	MOVL	$number,%EAX  ;\
	MOVL	8(%ESP),%EBX  ;\
	MOVL	12(%ESP),%ECX ;\
	MOVL	16(%ESP),%EDX ;\
	INT     $0x80         ;\
	POPL	%EBX          ;\
	RET

/* the system call library wrappers */
DO_CALL(system_halt, SYS_HALT)
DO_CALL(system_execute, SYS_EXECUTE)
DO_CALL(system_read, SYS_READ)
DO_CALL(system_write, SYS_WRITE)
DO_CALL(system_open, SYS_OPEN)
DO_CALL(system_close, SYS_CLOSE)

