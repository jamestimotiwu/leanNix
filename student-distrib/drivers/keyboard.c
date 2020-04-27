#include "keyboard.h"
#include "terminal.h"

//default status of special keys
// 0 denotes released ( unpressed ) state 
// 1 denotes pressed state 
//statically declared in orde to preserve the value between interrupt calls
static int caps = Released;
static int ctrl = Released;
static int alt = Released;
static int shiftL = Released;
static int shiftR = Released;


// length of current keyboard buffer 
volatile int cur_buf_length = 0;

//keyboard buffer 
volatile char kb_buf[KB_BUF_SIZE];

#define STATUS_CAPSLOCK 0x1
#define STATUS_SHIFT    0x2

// keyboard status 
// 0 = caps_lock unpressed + shift unpressed (0x00)
// 1 = caps_lock pressed   + shift unpressed (0x01)
// 2 = caps_lock unpreesed + shift pressed   (0x10)
// 3 = caps_lock pressed   + shift pressed   (0x11)
volatile int key_stat = 0;



#define KEY_STAT_SIZE 4
#define MAX_KEY_RANGE 60

/* Mapping for the keyboard codes */
// Don't prrint anything on hitting a functional key such as shift,alt...etc
char kb_map[KEY_STAT_SIZE][MAX_KEY_RANGE] = {

    // caps_lock unpressed + shift unpressed 
    {NOT_PRINT, NOT_PRINT, '1', '2', '3', '4','5', '6', '7', '8', '9', '0', '-', '=', NOT_PRINT, NOT_PRINT,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', NOT_PRINT, NOT_PRINT, 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l' , ';', '\'', '`', NOT_PRINT, '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm',',', '.', '/', NOT_PRINT, '*', NOT_PRINT, ' ', NOT_PRINT},

    // caps_lock pressed + shift unpressed 
    {NOT_PRINT, NOT_PRINT, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', NOT_PRINT, NOT_PRINT,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', NOT_PRINT, NOT_PRINT, 'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L' , ';', '\'', '`', NOT_PRINT, '\\', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', ',', '.', '/', NOT_PRINT, '*', NOT_PRINT, ' ', NOT_PRINT},

    // caps_lock unpreesed + shift pressed 

    {NOT_PRINT, NOT_PRINT, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', NOT_PRINT, NOT_PRINT,
        'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', NOT_PRINT, NOT_PRINT, 'A', 'S',
        'D', 'F', 'G', 'H', 'J', 'K', 'L' , ':', '"', '~', NOT_PRINT, '|', 'Z', 'X', 'C', 'V',
        'B', 'N', 'M', '<', '>', '?', NOT_PRINT, '*', NOT_PRINT, ' ', NOT_PRINT},


    // caps_lock pressed + shift pressed 
    {NOT_PRINT, NOT_PRINT, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', NOT_PRINT, NOT_PRINT,
        'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '{', '}', NOT_PRINT, NOT_PRINT, 'a', 's',
        'd', 'f', 'g', 'h', 'j', 'k', 'l' , ':', '"', '~', NOT_PRINT, '\\', 'z', 'x', 'c', 'v',
        'b', 'n', 'm', '<', '>', '?', NOT_PRINT, '*', NOT_PRINT, ' ', NOT_PRINT}


};

/* keyboard_int()
 *   DESCRIPTION: interrupt handler for the keyboard
 *   INPUT/OUTPUT: none
 *   SIDE EFFECTS: defines the functional key status 
 */
void keyboard_int(){

    cli();
    
    /* Get the scancode */
    uint8_t temp_sc; 
    uint8_t sc = inb(KB_DATA);  //get keybaord input 
    uint8_t pressedChar;
    //defines and handles the state of funtional keys 
    temp_sc = sc; 

    /* Keeps track if caps/shift/alt was pressed */
    int updateKeyStateFlag = 1;

    /* Check for shift/caps/alt key press/release */
    if(temp_sc == CAPS_LOCK){
        /* Swap the caps lock state */
        if (caps == Pressed)
            caps = Released;
        else
            caps= Pressed; 

    } else if(temp_sc == ALT_P) {
        alt = Pressed;
    } else if(temp_sc== ALT_R){
        alt = Released;
    } else if(temp_sc == CTRL_P){ 
        ctrl = Pressed;
    } else if(temp_sc == CTRL_R){
        ctrl = Released;
    } else if (temp_sc == SHIFT_L_P) {
        shiftL = Pressed;
    } else if (temp_sc == SHIFT_L_R) {
        shiftL = Released;
    } else if (temp_sc == SHIFT_R_P) {
        shiftR = Pressed;
    } else if (temp_sc == SHIFT_R_R) {
        shiftR = Released;
    } else {
        /* It was none of the above - don't update status */
        updateKeyStateFlag = 0;
    }

    if (updateKeyStateFlag) {

        /* Update key_stat, in case the key was shift/caps */
        key_stat = 0;

        if (shiftR == Pressed || shiftL == Pressed)
            key_stat |= STATUS_SHIFT;

        if (caps == Pressed)
            key_stat |= STATUS_CAPSLOCK;

        send_eoi(KB_IRQ);  //send EOI signal when done handling 
        sti();
        return; // exit from function execution (nothing left to print/do)
    }


    if(temp_sc == TAB){

        tab_func();    //call tab function 
    }
    else if(temp_sc == ENTER){

        enter_func();     // call enter function 
    }
    else if(temp_sc == BACK_SPACE){

        backsp_func();   //call backspace function 
    }
    else if(temp_sc == F1 && alt == Pressed){

        show_terminal(TERM1);
        send_eoi(KB_IRQ);
        sti();
        return;

    }
    else if(temp_sc == F2 && alt == Pressed){
        show_terminal(TERM2);
        send_eoi(KB_IRQ);
        sti();
        return;


    }
    else if(temp_sc ==F3 && alt == Pressed){

        show_terminal(TERM3);
        send_eoi(KB_IRQ);
        sti();
        return;


    }
    else if(temp_sc == CHAR_L && ctrl == Pressed){

        /* Ctrl-L pressed: clear the screen */
        term_clear(display_term);
        term_showbuf();

        send_eoi(KB_IRQ);  //send EOI signal when done handling 
        sti();
        return;
    }

    pressedChar = (uint8_t) get_char_map(temp_sc);  //else simply print char L to the screen
    /* check to make sure the character is printable */
    if (pressedChar != NOT_PRINT && pressedChar != NULL_BYTE)
        term_keyboardChar(pressedChar);


    send_eoi(KB_IRQ);  //send EOI signal when done handling 
    sti();
}


/* keyboard_init()
 *   DESCRIPTION: initializes the keyboard
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: enables irq associated with keyboard
 */
void keyboard_init(){
    enable_irq(KB_IRQ);
    cur_buf_length=0;
    //puts("KB initialized");
}



/* void get_char_map(char sc)
 *   DESCRIPTION: get char from the kb_map
 *   INPUT: sc --  scancode from keyboard
 *   OUTPUT: the ascii character code to be printed
 *   SIDE EFFECTS: outputs character to the screen/ update the keyboard buffer 
 */
char get_char_map(uint8_t sc){

    /* If it isn't within range, then don't print it */
    if (sc >= MAX_KEY_RANGE)
        return NOT_PRINT;

    /* otherwise, use lookup table to find correct character */
    return kb_map[key_stat][sc];

}



/* void reset_kb_buf()
 *   DESCRIPTION: resets and clear buffer
 *   INPUT/OUTPUT: None
 *   SIDE EFFECTS: resets and clears buffer
 */
void reset_kb_buf(int term_num){

    int i; 
    /* set everything in buffer to 0 */
    for(i=0; i< KB_BUF_SIZE; i++){

        terminals[term_num].kb_buf[i] = NULL_BYTE;   


    }

    // mark the buffer as empty
    terminals[term_num].kb_pos =0;  


}


/* void tab_func()
 *   DESCRIPTION: function for the tab when it's pressed 
 *   INPUT/OUTPUT: None
 *   SIDE EFFECTS: changes video mem
 */

void tab_func(){

    /* this function calculates correct num of spaces to print */
    term_keyboardTab();
}


/* void enter_func()
 *   DESCRIPTION: function for the enter when it's pressed 
 *   INPUT/OUTPUT: None
 *   SIDE EFFECTS: changes video mem
 */
void enter_func(){

    term_keyboardEnter();
}

/* void backsp_func()
 *   DESCRIPTION: function for the backspace when it's pressed 
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: function that handles the backspace from keyboard 
 */
void backsp_func(){

    term_keyboardBackspace();

}






