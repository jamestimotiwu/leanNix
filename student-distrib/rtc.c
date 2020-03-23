#include "rtc.h"

/* rtc_int
 *   DESCRIPTION: rtc interrupt handler test
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECT: prints to screen
 */
void rtc_int(){

  cli();				/* Disable interrupts */
	outb(0x8C, 0x70);	/* Select and read register C */
	inb(0x71);			/* Reset contents in CMOS port */
	send_eoi(8);		/* Send EOI for IRQ8 */
  //test_interrupts();
  printf("testing if rtc works");
	sti();
}

/* rtc_init()
 *   DESCRIPTION: initializes the rtc
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECT: changes rtc state, enables irq8
 */
void rtc_init(){

  char prev;
  cli();						/* Disable interrupts */
  outb(0x8B, 0x70);			/* Select register B, disable non-maskable interrupt(NMI) */
  prev = inb(0x71);			/* Read value of register B */
  outb(0x8B, 0x70);			/* Set index again because read resets index in register */
  outb((prev | 0x40), 0x71);	/* Write previous value | 0x40 to turn bit 6 of register in enable ints*/
  sti();						/* Enable interrupts */
  enable_irq(8);
}
