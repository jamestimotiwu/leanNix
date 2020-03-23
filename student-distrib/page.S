# page.S - Set up page directories, page table
# vim:ts=4 noexpandtab

#define ASM     1
#include "page.h"

.globl page_init

page_init:
	# Init stack frame
	pushl %ebp
	movl %esp, %ebp

	# Initialize page directory by loading CR3
	movl 8(%ebp), %eax
	movl %eax, %cr3

	# Set PSE for 4mib pages 
	movl %cr4, %eax
	orl $0x00000010, %eax
	movl %eax, %cr4

	# Set PG, PE bits of cr0
	movl %cr0, %eax
	orl $0x80000001, %eax
	movl %cr0, %eax

	leave
	ret