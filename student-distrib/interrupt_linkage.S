#define ASM 1
#include "syscall.h"

.text
.globl rtc_interrupt_assembly
.globl keyboard_interrupt_assembly
.globl syscall_assembly

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

syscall_jump_table:
    .long syscall_halt
    .long syscall_execute
    .long syscall_read
    .long syscall_write
    .long syscall_open
    .long syscall_close

# syscall interrupt
syscall_assembly:
  # push all registers except for eax, because that is expected to change
  pushl %ebp
  pushl %edi
  pushl %esi
  pushl %edx
  pushl %ecx
  pushl %ebx

  # jump table index starts at 0, but eax index starts at 1
  addl $-1, %eax
  cmpl $NUM_SYSCALLS, %eax
  jae syscall_error


  # not all system calls use all 3 arguments, but for simplicity use
  # the same code for all the system calls
  pushl %edx # 3rd arg
  pushl %ecx # 2nd arg
  pushl %ebx # 1st arg
  # call the function corresponding with eax register
  call *syscall_jump_table(,%eax,4)
  addl $12, %esp

  popl %ebx
  popl %ecx
  popl %edx
  popl %esi
  popl %edi
  popl %ebp
  iret

syscall_error:
  movl $-1, %eax # invalid syscall number: return -1
  popl %ebx
  popl %ecx
  popl %edx
  popl %esi
  popl %edi
  popl %ebp
  iret

.end
