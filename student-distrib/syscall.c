#include "lib.h"

/* syscall_int()
 *   DESCRIPTION: sys call c code
 *   INPUT/OUTPUT: none
 *   SIDE EFFECTS: prints to screen
 */
void syscall_int() {
    printf("SYSTEM CALL!\n");
}

PCB_t* get_PCB(){
  PCB_t* position;
  /* AND ESP with mask*/
  asm volatile(
    "andl %%esp, %%eax"
    :"=a"(position) /* output */
    :"a"(PCB_MASK)  /* input */
  );
  return position;
}
