#include "x86_desc.h"
#include "interrupts.h"
#include "lib.h"

void divide_error(void) {
	printf("Divide error exception.");
	while (1); // Prevent shutdown
}

void nmi(void) {
	printf("Non-maskable interrupt exception.");
	while (1);
}

void breakpoint(void) {
	printf("Breakpoint exception.");
	while (1);
}

void overflow(void) {
	printf("Overflow exception.");
	while (1);
}

void bounds(void) {
	printf("Bound range exceeded exception");
	while (1);
}

void invalid_op(void) {
	printf("Invalid Opcode exception.");
	while (1);
}

void device_not_available(void) {
	printf("Device not available exception.");
	while (1); 
}

void double_fault(void) {
	printf("Double fault exception.");
	while (1);
}

void coprocessor_segment_overrun(void) {
	printf("Coprocessor segment overrun exception.");
	while (1); 
}

void invalid_tss(void) {
	printf("Invalid TSS exception.");
	while (1);
}

void segment_not_present(void) {
	printf("Segment not present exception.");
	while (1);
}

void stack_segment(void) {
	printf("Stack segment fault exception.");
	while (1);
}

void general_protection(void) {
	printf("General protection exception.");
	while (1);
}

void page_fault(void) {
	printf("Page fault exception");
	while (1);
}

void math_fault(void) {
	printf("Math fault exception.");
	while (1);
}

void alignment_check(void) {
	printf("Alignment check exception");
	while (1);
}

void machine_check(void) {
	printf("Machine check exception");
	while (1);
}

void simd_coprocessor_check(void) {
	printf("SIMD floating point exception.");
	while (1);
}

void intel_reserved(void) {
	printf("Intel reserved exception");
}

/*
* set_gate
* Arguments:	handler_addr - pointer to interrupt handler function
*				gate_type - 0 for interrupt, 1 for trap
*				dpl - 0 for kernel privilege, 1 for user privilege
*				seg - segment selector
*				present - check if present
*				vec_num - interrupt vector
*/
void set_gate_desc(void* handler_addr, int gate_type, int dpl, int seg, int present, int vec_num)
{
	idt_desc_t gate_desc;

	/* Gate type */
	gate_desc.reserved4 = 0;
	/* Reserved3 -> int or trap*/
	gate_desc.reserved3 = gate_type;
	gate_desc.reserved2 = 1;
	gate_desc.reserved1 = 1;
	gate_desc.reserved0 = 0;
	/* DPL */
	gate_desc.dpl = dpl;
	/* Present */
	gate_desc.present = present;
	/* Segment selector */
	gate_desc.seg_selector = seg;

	/* Offset 15:00 lower address */
	/* Offset 31:16 upper address */
	SET_IDT_ENTRY(gate_desc, handler_addr);
	
	/* Set idt at vector */
	idt[vec_num] = gate_desc;
}

/*
* set_intr_desc
* Arguments: handler_addr - pointer to interrupt handler function
*			 irq_num - interrupt vector
*/
void set_interrupt_gate_desc(int vec_num, void* handler_addr)
{
	/* Reserved for interrupts: 0 */
	set_gate_desc(handler_addr, 0, 0, KERNEL_CS, 1, vec_num);
}

/*
* set_trap_desc
* Arguments: handler_addr - pointer to interrupt handler function
*			 irq_num - interrupt vector
*/
void set_trap_gate_desc(int vec_num, void* handler_addr)
{
	/* Reserved for interrupts: 0 */
	set_gate_desc(handler_addr, 1, 0, KERNEL_CS, 1, vec_num);
}

/*
* idt_init
*/
void idt_init(void)
{
	lidt(idt_desc_ptr);
	set_interrupt_gate_desc(0, divide_error);
	set_interrupt_gate_desc(1, intel_reserved);
	set_interrupt_gate_desc(2, nmi);
	set_interrupt_gate_desc(3, breakpoint);
	set_interrupt_gate_desc(4, overflow);
	set_interrupt_gate_desc(5, bounds);
	set_interrupt_gate_desc(6, invalid_op);
	set_interrupt_gate_desc(7, device_not_available);
	set_interrupt_gate_desc(8, double_fault);
	set_interrupt_gate_desc(9, coprocessor_segment_overrun);
	set_interrupt_gate_desc(10, invalid_tss);
	set_interrupt_gate_desc(11, segment_not_present);
	set_interrupt_gate_desc(12, stack_segment);
	set_interrupt_gate_desc(13, general_protection);
	set_interrupt_gate_desc(14, page_fault);
	set_interrupt_gate_desc(15, intel_reserved);
	set_interrupt_gate_desc(16, math_fault);
	set_interrupt_gate_desc(17, alignment_check);
	set_interrupt_gate_desc(18, machine_check);
	set_interrupt_gate_desc(19, simd_coprocessor_check);

}
