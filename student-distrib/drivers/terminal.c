#include "terminal.h"
#include "../types.h"
#include "../lib.h"

/* global variables used by terminal */
static int screen_x;
static int screen_y;
static char* video_mem = (char *)VIDEO1;

/* used by the read system call */
volatile static int readWaiting = 0;

volatile uint32_t current_term_num; 
terminal_t term[TERM_MAX];


// reference used: https://wiki.osdev.org/Text_Mode_Cursor

/* void reset_cursor(int x, int y)
 *   DESCRIPTION: update the cursor position on the screen
 *   INPUTS: screen_x position, screen_y position 
 *   OUTPUTS: none
 *   SIDE EFFECTS: function that handles the screen scroll-move 
 *                 video mem texts up by 1 if needed and clear the bottom line
 */
void reset_cursor(int x, int y){
    uint16_t position = y*NUM_COLS + x; 

    outb(0x0F, 0x3D4);
    outb((uint8_t) (position & 0xFF)  ,0x3D5);
    outb(0x0E, 0x3D4);
    outb((uint8_t) ((position >> 8) & 0xFF),0x3D5);
}


/* void term_init()
 *   DESCRIPTION: intialize all three terminals
 *   INPUT: none
 *   OUTPUT: none
 *   SIDE EFFECTS: initialize terminals 
 */
void term_init(){

  int i;
//initialize terminal struct for three terminals
  for(i=0; i< TERM_MAX; i++){

     term[i].x_cur =0; 
     term[i].y_cur =0; 
     term[i].flag =0;
     term[i].term_num=i;
     reset_kb_buf(i);

  }
 // initialize pointer to each terminal's video memory 
  term[TERM1].vid_mem = (char *) VIDEO1; 
  term[TERM2].vid_mem = (char *) VIDEO2; 
  term[TERM3].vid_mem = (char *) VIDEO3;

 current_term_num =0; //first terminal upon initialization 

 execute((uint8_t *) "shell"); //first terminal executing in shell upon initialization 


}


/* int32_t term_launch(int32_t term_num)
 *   DESCRIPTION: launch terminal
 *   INPUT: terminal number 
 *   OUTPUT: return -1 for failure, 0 for success
 *   SIDE EFFECTS: lauch specific terminal
 */
int32_t term_launch(int32_t term_number){

   /*if(term_number <0 || term_number >=TERM_MAX)
    return -1;  //check if terminal number is within range

   if(term_number == term_num) 
    return 0;  */ 

    if(term_number ==0) {

        printf("launch terminal 0");
    } 

    
    if(term_number ==1){

        printf("launch terminal 1");

    }


   if(term_number ==2){


     printf("launch terminal 2");
   }


    return 0;



}







/* term_clear
 *   DESCRIPTION: clears all video memory and cursor position
 *   INPUT: none
 *   OUTPUT: none
 *   SIDE EFFECTS: video memory is lost
 */
void term_clear() {
    int32_t i;

    /* clear all locations in video mem */
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
    }

    /* set cursor back to 0,0 */
    screen_x = 0;
    screen_y = 0;
    reset_cursor(0, 0);
}

/* term_test_int
 *   DESCRIPTION: used to test RTC; scrambles video memory
 *   INPUT: none
 *   OUTPUT: none
 *   SIDE EFFECTS: changes video mem
 */
void term_test_int() {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        video_mem[i << 1]++;
    }
}

/* term_scroll
 *   DESCRIPTION: scrolls the terminal down one line
 *   INPUT: none
 *   OUTPUT: none
 *   SIDE EFFECTS: changes videm memory
 */
void term_scroll() {
    int x,y, to, from;

    for (y = 1; y < NUM_ROWS; y++){
        for (x = 0; x < NUM_COLS; x++){

            from = x + y*NUM_COLS;
            to = x + (y-1)*NUM_COLS;

            video_mem[to << 1] = video_mem[from << 1];
        }
    }

    //clear up the last line
    y = NUM_ROWS - 1;
    for (x = 0; x < NUM_COLS; x++) {
        to = x + y*NUM_COLS;
        video_mem[to << 1] = ' ';
    }

    //update current position (left bottm of the screen)
    screen_x = 0; 
    screen_y = NUM_ROWS-1;
}

/* term_keyboardChar
 *   DESCRIPTION: prints a character and updates keyboard buffer
 *   INPUT: c -- the character
 *   OUTPUT: 1 if the buffer changes, otherwise 0
 *   SIDE EFFECT: changes keyboard buffer and display
 */
int term_keyboardChar(uint8_t c) {
    /* similar to term_putc, but this also effects the buffer */
    if (cur_buf_length >= KB_BUF_SIZE-1){
        // leave space for a newline
        return 0;
    }

    kb_buf[cur_buf_length++] = c;
    term_putc(c);
    // reset_cursor(screen_x, screen_y); do this in term_putc now
    return 1;
}

/* term_keyboardTab
 *   DESCRIPTION: prints a tab character
 *   INPUT: none
 *   OUTPUT: none
 *   SIDE EFFECT: prints 1-4 space characters
 */
void term_keyboardTab() {
    /* print a tab character */
    // calculate how many spaces left to print until next tabstop
    int spaces = 4 - (screen_x % TAB_STOP);

    while (spaces--)
        term_keyboardChar(' ');
}

/* term_keyboardBackspace
 *   DESCRIPTION: called when the backspace key is pressed
 *   INPUT: none
 *   OUTPUT: none
 *   SIDE EFFECT: changes keyboard buffer and display
 */
void term_keyboardBackspace() {
    // pass if buffer is empty or if cursor is in the top left
    if (cur_buf_length == 0 || (screen_x == 0 && screen_y == 0))
        return;


    // remove last character from buffer
    cur_buf_length--;

    // handle backspace on the same line
    if (screen_x >= 1) {
        screen_x--;
        term_setChar(' ');
    } else if (screen_y >= 0) {
        // handle the backspace on the line break
        screen_x = NUM_COLS-1;
        screen_y--;
        term_setChar(' ');
    }
    
    // set the cursor to last character position (ie. where the space is)
    reset_cursor(screen_x, screen_y);
}


/* term_keyboardEnter
 *   DESCRIPTION: called when the enter key is pressed
 *   INPUT: none
 *   OUTPUT: none
 *   SIDE EFFECT: changes keyboard buffer and display; read() syscall returns
 */
void term_keyboardEnter() {
    cli(); // so that readWaiting doesn't change its value unexpectedly

    kb_buf[cur_buf_length++] = '\n';
    term_putc('\n');
    // reset_cursor(0, screen_y); do this in term_putc now

    if (readWaiting) {
        readWaiting = 0;
        // don't change cur_buf_length because the read syscall will do that
    } else {
        /* reset the buffer by setting its length to 0 */
        cur_buf_length = 0;
    }
    sti();

}

/* term_showbuf
 *   DESCRIPTION: Prints out contents of keyboard buffer to screen
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: changes video mem
 */
void term_showbuf() {
    int i;
    for (i = 0; i < cur_buf_length; i++) {
        term_putc(kb_buf[i]);
    }
}


/* term_putc
 *   DESCRIPTION: prints a single character onto the terminal (not buffered)
 *   INPUT: c -- the character to print
 *   OUTPUT: none
 *   SIDE EFFECTS: changes video memory
 */
void term_putc(uint8_t c) {
    /* If c is null or other unprintable character, don't print it */
    if (c == NOT_PRINT || c == NULL_BYTE || c == '\r')
        return;

    if(c == '\n') {
        screen_y++;
        screen_x = 0;

        if (screen_y >= NUM_ROWS)
            term_scroll();

    } else {

        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
        screen_x++;

        if (screen_x >= NUM_COLS) {
            /* wrap around screen */
            screen_y++;
            screen_x = 0;
        }
        if (screen_y >= NUM_ROWS) {
            /* now y is off screen, so scroll down one line */
            term_scroll();
        }
    }
    /* puts the cursor in location of next character to print */
    reset_cursor(screen_x, screen_y);
}

/* term_setChar
 *   DESCRIPTION: writes a character to video mem at current cursor location
 *   INPUT: c -- the character
 *   OUTPUT: none
 *   SIDE EFFECT: changes video memory
 */
void term_setChar(uint8_t c) {
    *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
    *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
}

/* terminal_read
 *   DESCRIPTION: read the buffered input of the terminal
 *   INPUTS: fd -- file descriptor for the terminal
 *           buf -- destination for read data
 *           count -- how many bytes of data to read
 *   OUTPUTS: 0 for success, else -1
 *   SIDE EFFECTS: none
 */
int32_t terminal_read(int32_t fd, void *buf, uint32_t count) {
    int i;

    if (buf == NULL)
        return -1;

    readWaiting = 1;

    while (readWaiting) {
        // Do nothing; wait for this to exit (like spinlock)
        asm volatile("hlt"); // spin nicely
    }

    for (i = 0; i < count; i++) {
        if (i < cur_buf_length)
            ((uint8_t *)buf)[i] = kb_buf[i];
        else
            break;
    }
    if (i >= cur_buf_length) {
        // the read includes the final newline
        // Reset the buffer
        cur_buf_length = 0;
    }

    return i;

}

/* terminal_write
 *   DESCRIPTION: write to the terminal
 *   INPUTS: fd -- file descriptor for the terminal
 *           buf -- buffer 
 *           count -- how many bytes to write
 *   OUTPUTS: how many bytes were written, or -1 if failure
 *   SIDE EFFECTS: none
 */
int32_t terminal_write(int32_t fd, void *buf, uint32_t count) {
    int i;
    uint8_t *input = (uint8_t *) buf;

    if (buf == NULL)
        return -1;

    for (i = 0; i < count; i++) {

        /* write to terminal using standard function, not the keyboard function */
        term_putc(input[i]);

        // don't use term keyboard anymore -- that is only for keyboard input
        //if (term_keyboardChar(((uint8_t *)buf)[i]) == 0)
            /* no longer able to write to buffer, so exit */
        //    break;
    }
    /* return the number of bytes written */
    return i;
}


