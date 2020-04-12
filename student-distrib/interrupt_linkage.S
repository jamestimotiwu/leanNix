#define ASM 1
#include "syscall.h"
#include "x86_desc.h"

.text
.globl rtc_interrupt_assembly
.globl keyboard_interrupt_assembly
.globl syscall_assembly
.globl execute_iret, execute_iret_return

# These are interrupt handlers used directly by IDT

# rtc interrupt
rtc_interrupt_assembly:
  pushal # save all the registers
  pushfl # save all the flags
  call rtc_int
  popfl # restore flags
  popal # restore registers
  iret

# keyboard interrupt
keyboard_interrupt_assembly:
  pushal
  pushfl
  call keyboard_int
  popfl
  popal
  iret

# syscall interrupt
syscall_assembly:

# first, save all registers except for eax since eax will contain sys_call_table #
  pushl %ebp
  pushl %edi
  pushl %esi
  pushl %edx
  pushl %ecx
  pushl %ebx 

# check if the syscall # in in valid range (for cp3, 1~6)
  cmpl $1, %eax 
  jl invalid_syscall
  cmpl $NUM_SYSCALLS, %eax 
  jg invalid_syscall

# check syscall # is in valid range, push registers that will be used for arguments
  pushl %edx  # 3rd arg
  pushl %ecx  # 2nd arg
  pushl %ebx  # 1st arg
  call *syscall_table(,%eax,4)  # call function from syscall_table with syscall # stored in eax register
  addl $12, %esp # tear down args on stack
# restore all saved registers and return 
  popl %ebx
  popl %ecx
  popl %edx
  popl %esi
  popl %edi 
  popl %ebp
  iret



# For invalid_syscall, restore registers and return -1 (in eax) 
invalid_syscall: 
  movl $-1, %eax
  popl %ebx
  popl %ecx
  popl %edx
  popl %esi
  popl %edi 
  popl %ebp
  iret





# syscall jumptable 
syscall_table:
    .long 0x0, halt, execute, read, write, open, close


# arguments: esp, eip
execute_iret:
    pushl %ebp
    movl %esp, %ebp

    # setup the stack so that iret returns to user mode
    pushl $USER_DS # push user data segment
    movl 8(%ebp), %eax # new esp argument
    pushl %eax     # push esp
    pushfl         # push eflags (TODO IF?)
    pushl $USER_CS # push user cs
    movl 12(%ebp), %eax # new eip argument
    pushl %eax     # push eip

    iret
execute_iret_return:
    leave
    ret

.end
