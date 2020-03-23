#include "keyboard.h"
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
void keyboard_init(){
  enable_irq(KB_IRQ);
  //puts("KB initialized");
}
