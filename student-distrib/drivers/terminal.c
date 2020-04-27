#include "terminal.h"
#include "../types.h"
#include "../lib.h"
#include "../process.h"

/* global variables used by terminal */
static char* video_mem = (char *)VIDEO1;

/* used by the read system call */
static int readWaiting[TERM_MAX] = {0,0,0};

volatile uint32_t current_term_num; 
terminal_t terminals[TERM_MAX];

uint32_t display_term;

char video_mem_backup[TERM_MAX][VBUF_SIZE];

// reference used: https://wiki.osdev.org/Text_Mode_Cursor

/* void reset_cursor(int x, int y)
 *   DESCRIPTION: update the cursor position on the screen
 *   INPUTS: screen_x position, screen_y position 
 *   OUTPUTS: none
 *   SIDE EFFECTS: function that handles the screen scroll-move 
 *                 video mem texts up by 1 if needed and clear the bottom line
 */
void reset_cursor(uint32_t term, int x, int y){
    terminals[term].cur_x = x;
    terminals[term].cur_y = y;

    if (term == display_term) {
        uint16_t position = y*NUM_COLS + x; 

        outb(0x0F, 0x3D4);
        outb((uint8_t) (position & 0xFF)  ,0x3D5);
        outb(0x0E, 0x3D4);
        outb((uint8_t) ((position >> 8) & 0xFF),0x3D5);
    }
}

char video_mem_array[TERM_MAX][VBUF_SIZE];

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

        terminals[i].cur_x =0; 
        terminals[i].cur_y =0; 
        terminals[i].flag =0;
        terminals[i].term_num=i;
        // initialize pointer to each terminal's video memory 
        terminals[i].video_mem = video_mem_backup[i];
        reset_kb_buf(i);

    }

    terminals[0].video_mem = video_mem;
    display_term = 0; //first terminal upon initialization 

    //execute((uint8_t *) "shell"); //first terminal executing in shell upon initialization 
}


/* int32_t show_terminal
 *   DESCRIPTION: launch terminal
 *   INPUT: terminal number 
 *   OUTPUT: return -1 for failure, 0 for success
 *   SIDE EFFECTS: lauch specific terminal
 */
int32_t show_terminal(uint32_t term){

    if(term >=TERM_MAX)
        return -1;  //check if terminal number is within range

    /* Don't do anything if the terminal is already being displayed */
    if(term == display_term) 
        return 0;

    terminal_t *oldterm = &terminals[display_term];
    terminal_t *newterm = &terminals[term];

    cli();
    int i;
    for (i = 0; i < VBUF_SIZE; i++) {
        /* backup old display_term data */
        video_mem_backup[display_term][i] = video_mem[i];
        /* copy the backup of newterm into video memory */
        video_mem[i] = newterm->video_mem[i]; 
    }
    oldterm->video_mem = video_mem_backup[display_term];
    newterm->video_mem = video_mem;
    
    /* Set cursor to correct position */
    display_term = term;
    reset_cursor(display_term, newterm->cur_x, newterm->cur_y);

    sti();


    return 0;
}


/* term_clear
 *   DESCRIPTION: clears all video memory and cursor position
 *   INPUT: term -- the terminal to clear
 *   OUTPUT: none
 *   SIDE EFFECTS: video memory is lost
 */
void term_clear(uint32_t term) {
    int32_t i;
    terminal_t *t = &terminals[term];

    /* clear all locations in video mem */
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(t->video_mem + (i << 1)) = ' ';
        *(uint8_t *)(t->video_mem + (i << 1) + 1) = ATTRIB;
    }

    /* set cursor back to 0,0 */
    reset_cursor(term, 0, 0);
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
 *   INPUT: term -- number of the terminal to scroll
 *   OUTPUT: none
 *   SIDE EFFECTS: changes videm memory
 */
void term_scroll(uint32_t term) {
    terminal_t *t = &terminals[term];

    int x,y, to, from;

    for (y = 1; y < NUM_ROWS; y++){
        for (x = 0; x < NUM_COLS; x++){

            from = x + y*NUM_COLS;
            to = x + (y-1)*NUM_COLS;

            t->video_mem[to << 1] = t->video_mem[from << 1];
        }
    }

    //clear up the last line
    y = NUM_ROWS - 1;
    for (x = 0; x < NUM_COLS; x++) {
        to = x + y*NUM_COLS;
        t->video_mem[to << 1] = ' ';
    }

    //update current position (left bottm of the screen)
    t->cur_x = 0; 
    t->cur_y = NUM_ROWS-1;
}

/* term_keyboardChar
 *   DESCRIPTION: prints a character and updates keyboard buffer
 *   INPUT: c -- the character
 *   OUTPUT: 1 if the buffer changes, otherwise 0
 *   SIDE EFFECT: changes keyboard buffer and display
 */
int term_keyboardChar(uint8_t c) {
    terminal_t *state = &terminals[display_term];
    /* similar to term_putc, but this also effects the buffer */
    if (state->kb_pos >= KB_BUF_SIZE-1){
        // leave space for a newline
        return 0;
    }

    state->kb_buf[state->kb_pos++] = c;
    term_putc(c, display_term);
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
    int spaces = 4 - (terminals[display_term].cur_x % TAB_STOP);

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
    terminal_t *state = &terminals[display_term];
    // pass if buffer is empty or if cursor is in the top left
    if (state->kb_pos == 0 || (state->cur_x == 0 && state->cur_y == 0))
        return;


    // remove last character from buffer
    state->kb_pos--;

    // handle backspace on the same line
    if (state->cur_x >= 1) {
        state->cur_x--;
        term_setChar(' ', state->cur_x, state->cur_y);
    } else if (state->cur_y >= 0) {
        // handle the backspace on the line break
        state->cur_x = NUM_COLS-1;
        state->cur_y--;
        term_setChar(' ', state->cur_x, state->cur_y);
    }
    
    // set the cursor to last character position (ie. where the space is)
    reset_cursor(display_term, state->cur_x, state->cur_y);
}


/* term_keyboardEnter
 *   DESCRIPTION: called when the enter key is pressed
 *   INPUT: none
 *   OUTPUT: none
 *   SIDE EFFECT: changes keyboard buffer and display; read() syscall returns
 */
void term_keyboardEnter() {
    cli(); // so that readWaiting doesn't change its value unexpectedly
    terminal_t *t = &terminals[display_term];

    t->kb_buf[t->kb_pos++] = '\n';
    term_putc('\n', display_term);

    if (readWaiting[display_term]) {
        readWaiting[display_term] = 0;
        // don't change cur_buf_length because the read syscall will do that
    } else {
        /* reset the buffer by setting its length to 0 */
        t->kb_pos = 0;
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
    terminal_t *t = &terminals[display_term];
    int i;
    for (i = 0; i < t->kb_pos; i++) {
        term_putc(t->kb_buf[i], display_term);
    }
}


/* term_putc
 *   DESCRIPTION: prints a single character onto the terminal (not buffered)
 *   INPUT: c -- the character to print
 *          term -- the terminal to print to
 *   OUTPUT: none
 *   SIDE EFFECTS: changes video memory
 */
void term_putc(uint8_t c, uint32_t term) {
    /* If c is null or other unprintable character, don't print it */
    if (c == NOT_PRINT || c == NULL_BYTE || c == '\r')
        return;
    
    
    terminal_t *t = &terminals[term];

    if(c == '\n') {
        t->cur_y++;
        t->cur_x = 0;

        if (t->cur_y >= NUM_ROWS)
            term_scroll(term);

    } else {

        *(uint8_t *)(video_mem + ((NUM_COLS * t->cur_y + t->cur_x) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS * t->cur_y + t->cur_x) << 1) + 1) = ATTRIB;
        t->cur_x++;

        if (t->cur_x >= NUM_COLS) {
            /* wrap around screen */
            t->cur_y++;
            t->cur_x = 0;
        }
        if (t->cur_y >= NUM_ROWS) {
            /* now y is off screen, so scroll down one line */
            term_scroll(term);
        }
    }
    /* puts the cursor in location of next character to print */
    reset_cursor(term, t->cur_x, t->cur_y);
}

/* term_setChar
 *   DESCRIPTION: writes a character to video mem at current cursor location
 *   INPUT: c -- the character
 *          (x, y) -- coordinates
 *   OUTPUT: none
 *   SIDE EFFECT: changes video memory
 */
void term_setChar(uint8_t c, int x, int y) {
    *(uint8_t *)(video_mem + ((NUM_COLS * y + x) << 1)) = c;
    *(uint8_t *)(video_mem + ((NUM_COLS * y + x) << 1) + 1) = ATTRIB;
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

    uint32_t current_term = get_current_terminal();
    /* terminal read always reads from the current terminal */
    readWaiting[current_term] = 1;
    terminal_t *t = &terminals[current_term];

    while (readWaiting[current_term]) {
        // Do nothing; wait for this to exit (like spinlock)
        asm volatile("hlt"); // spin nicely
    }

    for (i = 0; i < count; i++) {
        if (i < t->kb_pos)
            ((uint8_t *)buf)[i] = t->kb_buf[i];
        else
            break;
    }
    if (i >= t->kb_pos) {
        // the read includes the final newline
        // Reset the buffer
        t->kb_pos = 0;
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
        term_putc(input[i], get_current_terminal());
    }
    /* return the number of bytes written */
    return i;
}


