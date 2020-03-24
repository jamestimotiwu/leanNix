#include "idt.h"
#include "syscall.h"

/* init_idt
 *   DESCRIPTION: clears all entires in the IDT, then initializes some of
 *                them to the correct interrupt handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: changes idt, idtr
 */
void init_idt() {
	int i;
	/* Initialize IDT */
	for (i = 0; i < NUM_VEC; i++) {
		/* Manual Figure 5-2 */
		idt[i].reserved0 = 0;
		idt[i].reserved1 = 1;
		idt[i].reserved2 = 1;
		idt[i].reserved3 = 0;
		idt[i].reserved4 = 0;
		/* Size of gate 1:32 bits, 0: 16 bits */
		idt[i].size = 1;
		/* DPL set to 0 */
		idt[i].dpl = 0;
		/* Present set to 0 */
		idt[i].present = 0;
		/* Set segment selector to kernel's code segment selector */
		idt[i].seg_selector = KERNEL_CS;

		/* 0 ~ 31 reserved by IA-32 architecture */
		if (i < RESERVED_NUMS) {
			idt[i].present = 1;
			SET_IDT_ENTRY(idt[i], reserved);
		}
		/* Only the system call has privilige of 3 */
		if (i == IDT_SYSTEM_CALL) {
			idt[i].dpl = 3;
			idt[i].present = 1;
		}
		if (i == IDT_KEYBOARD || i == IDT_RTC) {
			idt[i].present = 1;
		}
	}

	/* Initialize the exceptions */
	SET_IDT_ENTRY(idt[IDT_DIVIDE_ERROR], divide_error_exception);
	SET_IDT_ENTRY(idt[IDT_DEBUG], debug_exception);
	SET_IDT_ENTRY(idt[IDT_NMI], nmi_interrupt);
	SET_IDT_ENTRY(idt[IDT_BREAKPOINT], breakpoint_exception);
	SET_IDT_ENTRY(idt[IDT_OVERFLOW], overflow_exception);
	SET_IDT_ENTRY(idt[IDT_BOUND_RANGE_EXCEEDED], bound_range_exceeded_exception);
	SET_IDT_ENTRY(idt[IDT_INVALID_OPCODE], invalid_opcode_exception);
	SET_IDT_ENTRY(idt[IDT_DEVICE_NOT_AVAILABLE], device_not_available_exception);
	SET_IDT_ENTRY(idt[IDT_DOUBLE_FAULT], double_fault_exception);
	SET_IDT_ENTRY(idt[IDT_COPROCESS_SEGMENT_OVERRUN], coprocessor_segment_overrun_exception);
	SET_IDT_ENTRY(idt[IDT_INVALID_TSS], invalid_tss_exception);
	SET_IDT_ENTRY(idt[IDT_SEGMENT_NOT_PRESENT], segment_not_present_exception);
	SET_IDT_ENTRY(idt[IDT_STACK_FAULT], stack_fault_exception);
	SET_IDT_ENTRY(idt[IDT_GENERAL_PROTECTION], general_protection_exception);
	SET_IDT_ENTRY(idt[IDT_PAGE_FAULT], page_fault_exception);
	SET_IDT_ENTRY(idt[IDT_FPU_FLOATING_POINT], fpu_floating_point_error);
	SET_IDT_ENTRY(idt[IDT_ALIGNMENT_CHECK], alignment_check_exception);
	SET_IDT_ENTRY(idt[IDT_MACHINE_CHECK], machine_check_exception);
	SET_IDT_ENTRY(idt[IDT_SIMD_FLOATING_POINT], simd_floating_point_exception);

	/* Set handlers for the keyboard and rtc (hardware interrupts) */
	SET_IDT_ENTRY(idt[IDT_KEYBOARD], keyboard_interrupt_assembly);
	SET_IDT_ENTRY(idt[IDT_RTC], rtc_interrupt_assembly);
	/* set handler for system call */
	SET_IDT_ENTRY(idt[IDT_SYSTEM_CALL], syscall_assembly);

	/* set idtr */
	lidt(idt_desc_ptr);
}
