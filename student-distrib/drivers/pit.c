#include "pit.h"
#include "../lib.h"


/* pit_init
 *   DESCRIPTION: pit initializer
 *   INPUTS: frequency
 *   OUTPUTS: none
 *   SIDE EFFECT: None
 */
void pit_init(uint32_t frequency){
	/* calculate divisor to be sent to pit */
	uint32_t divisor = PIT_BASE/frequency;
	/* send command byte */
	outb(PIT_COMMAND_BYTE, PIT_COMMAND);
	/* divisor has to be sent byte-wise, so split between upper/lower bytes */
	outb(divisor & PIT_MASK, PIT_DATA_CHANNEL_0);
	outb((divisor>>8) & PIT_MASK, PIT_DATA_CHANNEL_0);
	/* send eoi for IRQ 0 */
	enable_irq(PIT_IRQ);
}
/* pit_int
 *   DESCRIPTION: pit interrupt handler
 *   INPUTS: None
 *   OUTPUTS: none
 *   SIDE EFFECT: None
 */
void pit_int(){
    cli();
    //printf("something");
    send_eoi(PIT_IRQ);
    sti();
}
