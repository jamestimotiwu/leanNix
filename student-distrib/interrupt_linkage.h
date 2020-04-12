#ifndef INTERRUPT_LINKAGE_H
#define INTERRUPT_LINKAGE_H

void rtc_interrupt_assembly();
void keyboard_interrupt_assembly();

void execute_iret(uint32_t esp, uint32_t eip);
void ret_to_user(int32_t eip);
void halt_ret(int32_t esp, int32_t ebp);

#endif
