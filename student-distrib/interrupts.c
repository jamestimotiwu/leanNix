#include "x86_desc.h"

void divide_error(void);
//void debug(void);
//void nmi(void);
//void overflow(void);
//void bounds(void);
//void invalid_op(void);
//void device_not_available(void);
//void coprocessor_segment_overrun(void);
//void invalid_tss(void);
//void device_not_available(void);
//void segment_not_present(void);
//void stack_segment(void);
//void general_protection(void);
//void page_fault(void);
//void coprocessor_error(void);
//void alignment_check(void);
//void machine_check(void);
//void simd_coprocessor_check(void);
//void machine_check(void);

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

	/* Offset 15:00 lower address */
	gate_desc.offset_15_00 = handler_addr & 0x0000ffff;
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
	/* Offset 31:16 upper address */
	gate_desc.offset_31_16 = handler_addr & 0xffff0000;
	/* Set idt at vector */
	idt[vec_num] = gate_desc;
}

/*
* set_intr_desc
* Arguments: handler_addr - pointer to interrupt handler function
*			 irq_num - interrupt vector
*/
void set_interrupt_gate_desc(void* handler_addr, int vec_num)
{
	/* Reserved for interrupts: 0 */
	set_gate_desc(handler_addr, 0, 0, KERNEL_CS, 1, vec_num);
}

/*
* set_trap_desc
* Arguments: handler_addr - pointer to interrupt handler function
*			 irq_num - interrupt vector
*/
void set_trap_gate_desc(void* handler_addr, int vec_num)
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
	set_interrupt_gate_desc(0, &divide_error);

}