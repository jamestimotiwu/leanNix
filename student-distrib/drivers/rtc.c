#include "rtc.h"

/* rtc_int
 *   DESCRIPTION: rtc interrupt handler test
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECT: prints to screen
 */
void rtc_int() {
	//cli();				/* Disable interrupts */
	test_interrupts();
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
	cli();						/* Disable interrupts */
	outb(REG_B, RTC_PORT);			/* Select register B, disable non-maskable interrupt(NMI) */
	unsigned char prev = inb(CMOS_PORT);			/* Read value of register B */
	outb(REG_B, RTC_PORT);			/* Set index again because read resets index in register */
	outb((prev | 0x40), CMOS_PORT);	/* Write previous value | 0x40 to turn bit 6 of register in enable ints*/
	sti();						/* Enable interrupts */
	enable_irq(RTC_IRQ);
	enable_irq(SLAVE_IRQ);
}
/* Sets frequency to desired hertz */
int rtc_set_freq(int freq){
	char new_rate;

	switch(freq){
		case 2:
			new_rate = _2_HZ;
			break;
		case 4:
			new_rate = _4_HZ;
			break;
		case 8:
			new_rate = _8_HZ;
			break;
		case 16:
			new_rate = _16_HZ;
			break;
		case 32:
			new_rate = _32_HZ;
			break;
		case 64:
			new_rate = _64_HZ;
			break;
		case 128:
			new_rate = _128_HZ;
			break;
		case 256:
			new_rate = _256_HZ;
			break;
		case 512:
			new_rate = _512_HZ;
			break;
		case 1024:
			new_rate = _1024_HZ;
			break;
		default:
			return -1;
			break;
	}
	/* Disable interrupts */
	cli();
	outb(REG_A, RTC_PORT);
	unsigned char prev = inb(CMOS_PORT);
	outb(REG_A, RTC_PORT);
	outb((prev & 0xF0) | new_rate, CMOS_PORT);
	/* Enable interrupts */
	sti();
	return 1;
}
/* Initialize RTC frequency to 2hz */
int rtc_open(){
	rtc_set_freq(2);
	return 0;
}
/* Does nothing unless RTC is virtualized */
int rtc_close(){
	return 0;
}
/* Blocks until next interrupt */
int rtc_read(){

}
/* Changes frequency */
int rtc_write(){

}
