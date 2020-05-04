#include "rtc.h"
#include "../process.h"


volatile int rtc_int_flag;

static int current_rtc_freq = -1;

/* rtc_int
 *   DESCRIPTION: rtc interrupt handler test
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECT: prints to screen
 */
void rtc_int() {
	//cli()
	
	//test_interrupts();
	outb(REG_C, RTC_PORT);	/* Select and read register C */
	inb(CMOS_PORT);			/* Reset contents in CMOS port */
	rtc_int_flag = 1;		/* Set interrupt flag */
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
	outb(REG_B, RTC_PORT);			/* Select register B, disable non-maskable interrupt(NMI) */
	unsigned char prev = inb(CMOS_PORT);			/* Read value of register B */
	outb(REG_B, RTC_PORT);			/* Set index again because read resets index in register */
	outb((prev | 0x40), CMOS_PORT);	/* Write previous value | 0x40 to turn bit 6 of register in enable ints*/
	rtc_set_freq(2);
	enable_irq(RTC_IRQ);
	enable_irq(SLAVE_IRQ);
}

/* rtc_set_freq
 *   DESCRIPTION Sets frequency to desired hertz
 *   INPUTS: freq -- the desired frequency
 *   OUTPUTS: -1 if error, 0 for success
 *   SIDE EFFECTS: changes RTC freq
 */
int rtc_set_freq(int freq){
	char new_rate;

    /* Don't do anything if rtc frequency already matches argument */
    if (freq == current_rtc_freq)
        return 0;

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
    current_rtc_freq = freq;
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

/* rtc_open
 *   DESCRIPTION: Initialize RTC frequency to 2hz
 *   INPUT: filename -- not currently used
 *   OUTPUT: 0 for success
 *   SIDE EFFECT: changes RTC frequency
 */
int rtc_open(const uint8_t* filename){
	rtc_set_freq(2);
    create_pcb(current_pid)->rtc_freq = 2;
	return 0;
}

/* rtc_close
 *   DESCRIPTION: Does nothing unless RTC is virtualized
 *   INPUTS: fd -- not yet used
 *   OUTPUTS: 0 for success
 */
int rtc_close(int32_t fd){
    create_pcb(current_pid)->rtc_freq = -1;
	return 0;
}

/* rtc_read
 *   DESCRIPTION: Blocks until next interrupt
 *   INPUTS: fd, buf, nbytes -- required for read syscall, but not used
 *   OUTPUTS: 0 for success
 *   SIDE EFFECTS: blocks execution
 */
int rtc_read(int32_t fd, void* buf, int32_t nbytes){
	rtc_int_flag = 0;
	while(rtc_int_flag == 0){
		/* Wait for rtc interrupt to return 0 */
	}
	rtc_int_flag = 0;
	return 0;
}

/* rtc_write
 *   DESCRIPTION: Changes frequency
 *   INPUTS: fd -- not used
 *           buf -- buffer containing the 4 byte int
 *           nbytes -- size of buffer (should be 4)
 *   OUTPUTS: number of bytes written, or -1 if failure
 */
int rtc_write(int32_t fd, const void* buf, int32_t nbytes){
	uint32_t newRate = *((int*)buf);
	int result = rtc_set_freq(newRate);

    if (result != -1)
        create_pcb(current_pid)->rtc_freq = newRate;

	return result;
	// if(result == -1){
	// 	return -1;
	// }
	// else{
	// 	return nbytes;
	// }
}
