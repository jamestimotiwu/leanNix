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

# syscall interrupt
syscall_assembly:
  pushal
  pushfl
  call syscall_int
  popfl
  popal
  iret

.end
