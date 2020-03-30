#include "terminal.h"
#include "keyboard.h"
#include "../types.h"
#include "../lib.h"

/* global variables used by terminal */
static int screen_x;
static int screen_y;
static char* video_mem = (char *)VIDEO;


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
}

void term_test_int() {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        video_mem[i << 1]++;
    }
}

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

void term_keyboardChar(uint8_t c) {
    /* similar to term_putc, but this also effects the buffer */
    if (cur_buf_length >= KB_BUF_SIZE-1){
        // leave space for a newline
        return;
    }

    kb_buf[cur_buf_length++] = c;
    term_putc(c);
}

void term_keyboardTab() {
    /* print a tab character */
}

void term_keyboardBackspace() {
    // pass
    if (cur_buf_length == 0)
        return;

    cur_buf_length--;

    if (screen_x >= 1) {
        screen_x--;
        term_setChar(' ');
    }
}

static int readWaiting = 0;

void term_keyboardEnter() {
    kb_buf[cur_buf_length++] = '\n';
    term_putc('\n');

    if (readWaiting) {

    }

    reset_kb_buf();

}

void term_putc(uint8_t c) {
    if(c == '\n' || c == '\r') {
        screen_y++;
        screen_x = 0;

        if (screen_y >= NUM_ROWS)
            term_scroll();

    } else {

        if (screen_x >= NUM_COLS) {
            /* wrap around screen */
            screen_y++;
            screen_x = 0;
        }
        if (screen_y >= NUM_ROWS) {
            /* now y is off screen, so scroll down one line */
            term_scroll();            
        }
        
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
        screen_x++;
        // screen_y = (screen_y + (screen_x / NUM_COLS)) % NUM_ROWS;
    }
}

void term_setChar(uint8_t c) {
    *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1)) = c;
    *(uint8_t *)(video_mem + ((NUM_COLS * screen_y + screen_x) << 1) + 1) = ATTRIB;
}

/* term_open
 *   DESCRIPTION: opens the terminal syscall
 *   INPUTS: fname -- filename for terminal driver
 *   OUTPUTS: 0 for success, else -1
 *   SIDE EFFECTS: none
 */
int32_t term_open(const uint8_t *fname) {
    return 0;
}

// TODO
int32_t term_close(int32_t fd) {
    return 0;
}

// TODO
int32_t term_read(int32_t fd, void *buf, uint32_t count) {
    return 0;
}

int32_t term_write(int32_t fd, void *buf, uint32_t count) {
    return 0;
}


