# boot.S - start point for the kernel after GRUB gives us control
# vim:ts=4 noexpandtab

#define ASM     1

.global load_page

load_page:
	pushl %ebp
	movl %esp, %ebp

    # 1. Load cr3 with address of page dir
    movl 8(%ebp), %eax
    movl %eax, %cr3

    # 2. Enable PSE page size extension bit of cr4
    movl %cr4, %eax
    orl $0x00000010, %eax
    movl %eax, %cr4

    # 3. Set PG paging flag bit of cr0
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0

    leave
    ret

flush_tlb:
    pushl %ebp
    movl %esp, %ebp
    
    # Reload cr3
    movl %cr3, %eax
    movl %eax, %cr3

    leave
    ret