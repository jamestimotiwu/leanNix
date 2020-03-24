#include "keyboard.h"

/* Mapping for the keyboard codes */
char kb_map[60] = {
  '\0', '0',
  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
  '-', '=', '0', '0',
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
  '[', ']', '0', '0',
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
  ';', '0', '0', '0', '0',
  'z', 'x', 'c', 'v', 'b', 'n', 'm',
  ',', '.', '/', '0', '0', '0', '0', '0', '0'
};

/* keyboard_int()
 *   DESCRIPTION: interrupt handler for the keyboard
 *   INPUT/OUTPUT: none
 *   SIDE EFFECTS: prints the character that was just pressed
 */
void keyboard_int(){
  // printf("KEYBOARD INTERRUPT! \n");
  // while(1);
  unsigned scancode = inb(KB_DATA);
  if(scancode < 60 && scancode >= 0){
    //clear();
    putc(kb_map[scancode]);
  }
  // clear();
  // putc(kb_map[scancode]);
  send_eoi(KB_IRQ);
}

/* keyboard_init()
 *   DESCRIPTION: initializes the keyboard
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: enables irq associated with keyboard
 */
void keyboard_init(){
  enable_irq(KB_IRQ);
  //puts("KB initialized");
}
