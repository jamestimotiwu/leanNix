#ifndef INTERRUPT_LINKAGE_H
#define INTERRUPT_LINKAGE_H

void rtc_interrupt_assembly();
void keyboard_interrupt_assembly();

void execute_iret(uint32_t esp, uint32_t eip);

#endif
