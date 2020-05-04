#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "../types.h"
#include "keyboard.h"
#include "../syscall.h"

#define VIDEO1       0xB8000
#define VID_BACKUP   0x01000 //4kb video memory backup space for each terminal
#define VIDEO_T1     0xB9000; 
#define VIDEO_T2 	 0xBA000;
#define VIDEO_T3     0xBB000;
#define NUM_COLS    80
#define NUM_ROWS    25
#define VBUF_SIZE   (NUM_COLS*NUM_ROWS*2)
#define ATTRIB      0x7
#define TERM_MAX   	3


#define TAB_STOP 4

/* basic terminal functionality */
void term_clear(uint32_t term);
void term_showbuf(int term); /* prints out current buffer contents */
void term_putc(uint8_t c, uint32_t term);
void term_test_int();
int32_t show_terminal(uint32_t term);
void term_init(); //initialize all three terminals 

//extern char video_mem_backup[TERM_MAX][VBUF_SIZE];

extern char* video_mem_backup[TERM_MAX];

/* struct for terminal */
typedef struct terminal{
 
 int32_t cur_x; 
 int32_t cur_y; 

 char kb_buf[KB_BUF_SIZE]; //keyboard buffer for each terminal
 int kb_pos;  //buf length for each terminal 

 int32_t flag; // to check if termianl is currently operating (1= running, 0= not running)
 int32_t term_num; // terminal number (0,1,2) 
 
 //char *vid_mem; //video memory ptr to each terminal 
 char *video_mem;


}terminal_t;


extern uint32_t display_term;
extern terminal_t terminals[TERM_MAX];



/* Functions used by keyboard */
int term_keyboardChar(uint8_t c);
void term_keyboardBackspace();
void term_keyboardEnter();
void term_keyboardTab();
void term_setChar(uint8_t c, int x, int y);

/* system calls */

int32_t terminal_read(int32_t fd, void *buf, uint32_t count);
int32_t terminal_write(int32_t fd, void *buf, uint32_t count);

#endif
