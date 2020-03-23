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

  /* Set mask */
  master_mask = MASK;
  slave_mask = MASK;

  /* Mask all*/
  outb(master_mask, MASTER_8259_DATA);
  outb(slave_mask, SLAVE_8259_DATA);

  /* Initialize Master */
  outb(ICW1, MASTER_8259_PORT);
  outb(ICW2_MASTER, MASTER_8259_DATA);
  outb(ICW3_MASTER, MASTER_8259_DATA);
  outb(ICW4, MASTER_8259_DATA);

  /* Initialize Slave */
  outb(ICW1, SLAVE_8259_PORT);
  outb(ICW2_SLAVE, SLAVE_8259_DATA);
  outb(ICW3_SLAVE, SLAVE_8259_DATA);
  outb(ICW4, SLAVE_8259_DATA);

  /* Restore IRQ mask */
  outb(master_mask, MASTER_8259_DATA);
  outb(slave_mask, SLAVE_8259_DATA);

}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
  /* Check for valid irq_num */
  if(irq_num < 0 || irq_num > IRQ_NUMS){
    return;
  }
  /* Master PIC */
  if(irq_num < MIDDLE){
    /* Set bit as unmasked */
    master_mask = master_mask & ~(1 << irq_num);
    outb(master_mask, MASTER_8259_DATA);
  }
  /* Slave PIC */
  else{
    /* Set bit as unmasked */
    slave_mask = master_mask & ~(1 << (irq_num - MIDDLE));
    outb(slave_mask, SLAVE_8259_DATA);
  }
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
  /* Check for valid irq_num */
  if(irq_num < 0 || irq_num > IRQ_NUMS){
    return;
  }
  /* Master PIC */
  if(irq_num < MIDDLE){
    /* Set bit as masked */
    master_mask = master_mask | (1 << irq_num);
    outb(master_mask, MASTER_8259_DATA);
  }
  /* Slave PIC */
  else{
    /* Set bit as masked */
    slave_mask = slave_mask | (1 << (irq_num - MIDDLE));
    outb(slave_mask, SLAVE_8259_DATA);
  }
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
  /* Check for valid irq_num */
  if(irq_num < 0 || irq_num > IRQ_NUMS){
    return;
  }
  /* Master PIC */
  if(irq_num < MIDDLE){
    /* EOI or with interrupt number and sent to PIC */
    outb((EOI | irq_num), MASTER_8259_PORT);
  }
  /* Slave PIC */
  else{
    /* Also need to send EOI to master if sending EOI to slave on cascade pin */
    outb((EOI | ICW3_SLAVE), MASTER_8259_PORT);
    /* EOI or with interrupt number and sent to PIC */
    outb((EOI | (irq_num - MIDDLE)), SLAVE_8259_PORT);
  }
}
