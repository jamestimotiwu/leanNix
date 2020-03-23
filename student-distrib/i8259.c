/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    master_mask = 0xff;
    slave_mask = 0xff;
    /* Mask master and slave PICs data*/
    outb(0xff, MASTER_8259_PORT + 1);
    outb(0xff, SLAVE_8259_PORT + 1);

    /* Control words */
    outb(ICW1, MASTER_8259_PORT); /* ICW1: select 8259A-1 as master to port*/
    outb(ICW1, SLAVE_8259_PORT); /* ICW1: select 8259A-1 as master to port*/

    /* ICW2: Map IRQs to IDT vectors (data) */
    outb(ICW2_MASTER, MASTER_8259_PORT + 1);
    outb(ICW2_SLAVE, SLAVE_8259_PORT + 1);

    /* ICW3: Tell cascade 8259A-1 (the master) has a slave on IRQ*/
    outb(ICW3_MASTER, MASTER_8259_PORT + 1);
    outb(ICW3_SLAVE, SLAVE_8259_PORT + 1);

    outb(ICW4, MASTER_8259_PORT + 1);
    outb(ICW4, SLAVE_8259_PORT + 1);

    /* Restore master ans slave masks*/
    outb(master_mask, MASTER_8259_PORT + 1);
    outb(slave_mask, SLAVE_8259_PORT + 1);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    unsigned int mask = ~(1 << irq_num);

    master_mask &= mask;
    
    if (irq_num > 8) {
        // Slave is IRQ 8-15
        mask = ~(1 << (irq_num - 8))
        slave_mask &= mask;
        outb(slave_mask, SLAVE_8259_PORT + 1);
    }
    else {
        outb(master_mask, MASTER_8259_PORT + 1);
    }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    unsigned int mask = 1 << irq_num;

    /* Remove masking and update */
    master_mask |= mask;

    if (irq_num > 8) {
        // Slave is IRQ 8-15
        mask = 1 << (irq_num - 8)
        slave_mask |= mask; /* Remove masking for slave */
        outb(slave_mask, SLAVE_8259_PORT + 1);
    }
    else {
        outb(master_mask, MASTER_8259_PORT + 1);
    }
}


/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    /* Check if irq_num is in slave or master and send eoi for selected PIC and IRQ */
    if (irq >= 8) {
        outb(EOI | (irq_num - 8), SLAVE_8259_PORT);
    }
    outb(EOI | irq_num, MASTER_8259_PORT); /* Always send EOI to master */
}
