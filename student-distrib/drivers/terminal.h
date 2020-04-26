#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "../types.h"
#include "keyboard.h"
#include "../syscall.h"

#define VIDEO1       0xB8000
#define VID_BACKUP   0x01000 //4kb video memory backup space for each terminal
#define VIDEO2       VIDEO1+VID_BACKUP; 
#define VIDEO3 		 VIDEO2+VID_BACKUP;
#define NUM_COLS    80
#define NUM_ROWS    25
#define ATTRIB      0x7
#define TERM_MAX   	3


#define TAB_STOP 4

/* basic terminal functionality */
void term_clear();
void term_showbuf(); /* prints out contents currently in buffer */
void term_putc(uint8_t c);
void term_test_int();
int32_t term_launch(int32_t term_number);
void term_init(); //initialize all three terminals 


/* struct for terminal */
typedef struct terminal{
 
 int32_t x_cur; 
 int32_t y_cur; 

 volatile char kb_buf[KB_BUF_SIZE]; //keyboard buffer for each terminal
 volatile int cur_buf_length;  //buf length for each terminal 

 int32_t flag; // to check if termianl is currently operating (1= running, 0= not running)
 int32_t term_num; // terminal number (0,1,2) 
 
 char *vid_mem; //video memory ptr to each terminal 


}terminal_t;


extern volatile uint32_t current_term_num; 
extern terminal_t term[TERM_MAX];



/* Functions used by keyboard */
int term_keyboardChar(uint8_t c);
void term_keyboardBackspace();
void term_keyboardEnter();
void term_keyboardTab();
void term_setChar(uint8_t c);

/* system calls */

int32_t terminal_read(int32_t fd, void *buf, uint32_t count);
int32_t terminal_write(int32_t fd, void *buf, uint32_t count);

#endif
