.text
.globl rtc_interrupt_assembly
.globl keyboard_interrupt_assembly


rtc_interrupt_assembly:
  pushal
  pushfl
  call rtc_int
  popfl
  popal
  iret

keyboard_interrupt_assembly:
  pushal
  pushfl
  call keyboard_int
  popfl
  popal
  iret

.end
