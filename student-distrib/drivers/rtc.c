#include "rtc.h"

/* rtc_int
 *   DESCRIPTION: rtc interrupt handler test
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECT: prints to screen
 */
void rtc_int() {
	//cli();				/* Disable interrupts */
	
	//test_interrupts();
	outb(REG_C, RTC_PORT);	/* Select and read register C */
	inb(CMOS_PORT);			/* Reset contents in CMOS port */
	send_eoi(RTC_IRQ);		/* Send EOI for IRQ8 */
  //printf("testing if rtc works");
	//sti();
}

/* rtc_init()
 *   DESCRIPTION: initializes the rtc
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECT: changes rtc state, enables irq8
 */
void rtc_init() {
	//cli();						/* Disable interrupts */
	outb(REG_B, RTC_PORT);			/* Select register B, disable non-maskable interrupt(NMI) */
	unsigned char prev = inb(CMOS_PORT);			/* Read value of register B */
	outb(REG_B, RTC_PORT);			/* Set index again because read resets index in register */
	outb((prev | 0x40), CMOS_PORT);	/* Write previous value | 0x40 to turn bit 6 of register in enable ints*/
	//sti();						/* Enable interrupts */
	enable_irq(RTC_IRQ);
	enable_irq(SLAVE_IRQ);
}
