#include "../lib.h"
#include "../i8259.h"
#include "rtc.h"

/*
* RTC i/o ports: RTC: 0x70, CMOS: 0x71
* RTC status registers A, B, C offsets IO ports for RTC/CMOS -> 0xA, 0xB, 0xC
*/
static unsigned long rtc_data;
static unsigned long rtc_status;

/* RTC driver initializiation */
void rtc_init(void) 
{
	char prev;
	cli();						/* Disable interrupts */
	outb(0x70, 0x8B);			/* Select register B, disable non-maskable interrupt(NMI) */
	prev = inb(0x71);			/* Read value of register B */
	outb(0x70, 0x8B);			/* Set index again because read resets index in register */
	outb(0x71, prev | 0x40);	/* Write previous value | 0x40 to turn bit 6 of register in enable ints*/
	sti();						/* Enable interrupts */
}

/* Handler for interrupts to IRQ8 */
void rtc_interrupt_handler(int irq_num)
{
	cli();				/* Disable interrupts */
	outb(0x70, 0x0C);	/* Select and read register C */
	inb(0x71);			/* Reset contents in CMOS port */
	send_eoi(8);		/* Send EOI for IRQ8 */
	sti();
}