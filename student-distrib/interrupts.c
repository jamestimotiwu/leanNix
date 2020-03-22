#include "x86_desc.h"

/*
	divide_error
	debug
	nmi
	int3
	overflow
	bounds
	invalid_op
	device_not_available
	coprocessor_segment_overrun
	invalid_tss
	segment_not_present
	stack_segment
	general_protection
	page_fault
	coprocessor_error
	simd_coprocessor_error
	alignment_check
	spurious_interrupt_bug
	machine_check
*/
void divide_error(void);
void debug(void);
void nmi(void);
void overflow(void);
void bounds(void);
void invalid_op(void);
void device_not_available(void);
void coprocessor_segment_overrun(void);
void invalid_tss(void);
void device_not_available(void);
void segment_not_present(void);
void stack_segment(void);
void general_protection(void);
void page_fault(void);
void coprocessor_error(void);
void alignment_check(void);
void machine_check(void);
void simd_coprocessor_check(void);
void machine_check(void);

void set_gate(void* handler_addr, int vec_num, int dpl, int seg, int present)
{
	/*
	uint16_t offset_15_00;
	uint16_t seg_selector;
	uint8_t  reserved4;
	uint32_t reserved3 : 1;
	uint32_t reserved2 : 1;
	uint32_t reserved1 : 1;
	uint32_t size : 1;
	uint32_t reserved0 : 1;
	uint32_t dpl : 2;
	uint32_t present : 1;
	uint16_t offset_31_16;*/
	idt_desc_t gate_desc;

	/* Offset 15:00 lower address */
	gate_desc.offset_15_00 = handler_addr & 0x0000ffff;
	/* DPL */
	gate_desc.dpl = dpl;
	/* Present */
	gate_desc.present = present;
	/* Segment selector */
	gate_desc.seg_selector = seg;
	/* Offset 31:16 upper address */
	gate_desc.offset_31_16 = handler_addr & 0xffff0000;
}
/*
* setup_idt
* Arguments: handler_addr - pointer to interrupt handler function
*			 irq_num - interrupt vector
*/
void set_int_gate(void* handler_addr, int irq_num)
{
	uint32_t idt_upper;
	uint32_t idt_lower;

	/* Offset 31:16 mask upper address, DPL set to 3, present set to 1, size values*/
	idt_upper = (handler_addr & 0xffff0000) | (1 << 15) | (3 << 13) | 0x0E00;

	/* Segment selector 15:00 */
	idt_lower = (KERNEL_CS << 16) | handler_addr & 0xffff;

	/* Fill entry into IDT */
	idt[32 + irq_num] = ((unsigned long long)(idt_upper) << 32) | idt_lower;
}

void idt_init(void)
{
}