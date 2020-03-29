#include "keyboard.h"
#include "terminal.h"
#include "../lib.h"

//default status of special keys
// 0 denotes released ( unpressed ) state 
//1 denotes pressed state 
//statically declared in orde to preserve the value between interrupt calls
static int caps =0;
static int ctrl = 0; 
static int alt = 0; 


// length of current keyboard buffer 
volatile int cur_buf_length = 0;

//keyboard buffer 
volatile char kb_buf[KB_BUF_SIZE];

// keyboard status 
// 0 = caps_lock unpressed + shift unpressed
// 1 = caps_lock pressed + shift unpressed 
// 2 = caps_lock unpreesed + shift pressed 
// 3 = caps_lock pressed + shift pressed 
volatile int key_stat = 0;



#define KEY_STAT 4

/* Mapping for the keyboard codes */
// Don't prrint anything on hitting a functional key such as shift,alt...etc
char kb_map[KEY_STAT][60] = {

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
    /*
    char temp_sc; 
    char sc = inb(KB_DATA);  //get keybaord input 

    if(sc < 1 || sc > 60) {

        return;                  // check if the scancode read is within normal boundary between 1~60

    }
    else {    //defines and handles the state of funtional keys 

        temp_sc = sc; 

        if(temp_sc == CAPS_LOCK){

            caps= Pressed; 

        }

        else if(temp_sc == ALT_P) {

            alt = Pressed;

        }

        else if(temp_sc== ALT_R){

            alt = Released;


        }

        else if(temp_sc == CTRL_P){ 

            ctrl = Pressed;

        }

        else if(temp_sc == CTRL_R){

            ctrl = Released;


        }



        else if(temp_sc== SHIFT_L_P || temp_sc == SHIFT_R_P ) {

            if(caps==Pressed){

                key_stat = 3;   //case where caps pressed + shift pressed; 
            }   

            else{

                key_stat = 2;  // case where caps released + shift pressed; 
            }

        }

        else if(temp_sc == SHIFT_L_R || temp_sc == SHIFT_R_R){

            if(caps==Pressed){

                key_stat = 1; // caps pressed + shift unpressed'


            }

            else{

                key_stat = 0;  //caps unpressed + shift unpressed 

            }



        }

        else if(temp_sc == TAB){

            tab_func();    //call tab function 
        }
        else if(temp_sc == ENTER){

            enter_func();     // call enter function 
        }
        else if(temp_sc == BACK_SPACE){

            backsp_func();   //call backspace function 
        }
        else if(temp_sc == CHAR_L ){

            if(ctrl == Pressed){

                //clear();   //if ctrl+L is pressed, clear screen(video mem)
                //screen_x =0;
                //screen_y =0;
                term_clear();
                reset_cursor(0, 0); //reset cursor at the upper left corner
                reset_kb_buf();   //reset keyboard buffer

            }
            else{

                get_char_map(temp_sc);  //else simply print char L to the screen
            }
        }
        else {
            get_char_map(temp_sc); 
        }
    }


    */
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
 *   DESCRIPTION: get char from the kb_map and outputs on to the screen.
 *   INPUT/OUTPUT: Input = scancode from keyboard
 *   SIDE EFFECTS: outputs character to the screen/ update the keyboard buffer 
 */

void get_char_map(char sc){

    /*
    char out = kb_map[key_stat][sc];

    if(out== NULL_BYTE){

        return;          //do not print null bytes

    }

    if(cur_buf_length >= KB_BUF_SIZE-1){   //buffer size = 128 = 127 + new line char ( new line is always added at the end of the buffer before return)

        return;             //if buffer fills up, return 
    }



    else{ 


        kb_buf[cur_buf_length++] = out;  //if cur_buf_length is less than max buffer size, add character to the keyboard buffer



        if((screen_x==NUM_COLS-1) && (screen_y==NUM_ROWS-1)){
            scroll_screen();   // if current position is at the end of screen (bottom right corner), scroll screen
            putc(out); 
            reset_cursor(screen_x, screen_y); 
        }
        else{

            putc(out); 

        }


    }
    */


}



/* void reset_kb_buf()
 *   DESCRIPTION: resets and clear buffer
 *   INPUT/OUTPUT: None
 *   SIDE EFFECTS: resets and clears buffer
 */

void reset_kb_buf(){

    int i; 
    for(i=0; i< KB_BUF_SIZE; i++){

        kb_buf[i] = NULL_BYTE;   


    }

    cur_buf_length =0;  


}


/* void tab_func()
 *   DESCRIPTION: function for the tab when it's pressed 
 *   INPUT/OUTPUT: None
 *   SIDE EFFECTS: fuction for the tab when it's pressed 
 */

void tab_func(){

    /*
    int tab_space =3;  //add 3 spaces on the terminal screen / buffer
    int x;
    char space = char_space; // ' '

    if(cur_buf_length >= KB_BUF_SIZE){


        return;      //return if current buffer size is larger thant max buf size
    }


    for(x=0 ; x< tab_space; x++){  
        kb_buf[cur_buf_length++] = space;  // fill keyboard buffer with space  



        video_mem[screen_y * NUM_COLS + screen_x] = space; //fill in spaces
        video_mem[screen_y * NUM_COLS + screen_x] = ATTRIB; //color black
        screen_x++;
        if((screen_x==NUM_COLS-1) && (screen_y==NUM_ROWS-1)){
            scroll_screen();   //if current position is at the end of screen (bottom right corner), scroll screen
        }
        reset_cursor(screen_x, screen_y);  //update the cursor position on the screen 


    }
    */



}


/* void enter_func()
 *   DESCRIPTION: function for the enter when it's pressed 
 *   INPUT/OUTPUT: None
 *   SIDE EFFECTS: fuction for the enter when it's pressed 
 */
void enter_func(){

    /*
    if(cur_buf_length >= KB_BUF_SIZE){
        return;         //check if current buffer length is greater than the max buf size 
    }



    screen_x = 0;   // x position always becomes 0 (start of the row) on the screen


    if(screen_y == NUM_ROWS -1){
        scroll_screen();          // if current y position is at the last line of screen, scroll screen
        reset_cursur(screen_x, screen_y); // update the cursur position (screen_x =0, screen_y = last line) 

    }
    else{ 
        screen_y++;  // increment y position of the screen by 1

        reset_cursur(screen_x, screen_y); //update cursur to new position

    }

    kb_buf[cur_buf_length++] = NEW_LINE; //always add newline char at the end of buffer
    */


}

/* void backsp_func()
 *   DESCRIPTION: function for the backspace when it's pressed 
 *   INPUTS: none
 *   OUTPUTS: none
 *   SIDE EFFECTS: function that handles the backspace from keyboard 
 */

void backsp_func(){


    /*
    if(((screen_x==0) && (screen_y==0)) || (cur_buf_length==0)){


        return;     // if the current position left top corner or keyboard buffer is empty, return
    }

    else{

        screen_x--;  //decrement x position by 1
        video_mem[screen_y * NUM_COLS + screen_x] = NULL_BYTE; //erase char from screen

    }

    cur_buf_length--;   // decrement keyboard buffer length by 1
    kb_buf[cur_buf_length] = NULL_BYTE;  //update buffer with null byte for deleted char
    reset_cursor(screen_x, screen_y);  // update the cursor position
    */

}










