#define ASM 1
#include "syscall.h"
#include "x86_desc.h"
#include "process.h"

.text
.globl rtc_interrupt_assembly
.globl keyboard_interrupt_assembly
.globl pit_interrupt_assembly
.globl syscall_assembly
.globl ret_to_user, halt_ret

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

# pit interrupt
pit_interrupt_assembly:
  sti # why is IF cleared when this starts?
  pushal
  pushfl
  call pit_int
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

  # set data segment DS to kernel's data segment
  pushl %ecx
  movl $KERNEL_DS, %ecx
  movw %cx, %ds
  popl %ecx

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

  movl $USER_DS, %ebx
  movw %bx, %ds # restore data segment to user data segment
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
  movl $USER_DS, %ebx
  movw %bx, %ds # restore data segment to user data segment

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
    .long 0x0, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn

# Return to user from execute, push user DS, esp, eflags, cs, eip
ret_to_user:
    pushl %ebp
    movl %esp, %ebp

    # iret stack frame: USER_DS, esp, elfags, USER_CS, eip

    # Set ds register to load to gdt entry
    movl $USER_DS, %eax
    movw %ax, %ds
    pushl %eax          # user_ds
    # esp of user program
    pushl $PROGRAM_VIRTUAL_STACK # program's stack
    # eflags
    pushfl               # flags
    # user cs
    movl $USER_CS, %eax
    #movw %ax, %cs
    pushl %eax           # user_cs
    movl 8(%ebp), %eax
    pushl %eax           # new eip (userspace)

    iret

halt_ret:
    pushl %ebp
    movl %esp, %ebp

    movl 8(%ebp), %ecx # put the esp in ecx
    movl 12(%ebp), %edx # put ebp in edx
    movl 16(%ebp), %eax # execute's return value
    movl %ecx, %esp
    movl %edx, %ebp
    sti
    leave
    ret

.end

