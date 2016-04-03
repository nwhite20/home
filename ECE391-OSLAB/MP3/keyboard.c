#include "keyboard.h"
#include "lib.h"

// handle left_ctrl3
// change clear to reset screen x and screen y

#define ctrl_val 0x1D
#define ctrl_release 0x9D              

#define alt_val 0x38
#define alt_release 0xB8

#define shift_val 0x2A
#define shift_release 0xAA
#define shift_release_right 0xB6

#define caps_val 0x3A
#define caps_release 0xBA

#define nall '\0'  
#define newline '\n'
#define backs '\b'
#define max_buff_size 127
#define aa 'a'
#define zz 'z'
#define ll 'l'
#define alpha_off 32


volatile uint8_t global_key=nall;


int32_t shift_flag=0;
int32_t alt_flag=0;
int32_t caps_flag=0;
int32_t ctrl_flag=0;


int32_t key_spam_flag=0;

static int32_t key_index=0;
static int32_t full=0;
static int32_t full_new=0;

int32_t backs_key=0;
int32_t rese_key=0;




/* table source: osdever.net */
unsigned char kbdus[NUM_SCANCODES] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    ctrl_val,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',  shift_val,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   shift_val,				/* Right shift */
  '*',
    alt_val,	/* Alt */
  ' ',	/* Space bar */
    caps_val,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};	


/*
* void keyboard_init();
*   Inputs: void
*   Return Value: none
*   Function: Unmasks keyboard interrupt line
*/
void keyboard_init() {
	
  /* unmasking keyboard interrupt line */
   /* kbdus[0x1D]=right_ctrl;
    kbdus[0x38]=right_alt; */
/*
     kbdus[0x1D+0x80]=right_ctrl;
    kbdus[0x38+0x80]=right_alt; */
	enable_irq(KEYBOARD_IRQ);

}

/* 0x64 is control for keyboard - future reference */

/*
* void do_keyboard();
*   Inputs: none
*   Return Value: none
*   Function: Processes a keyboard interrupt and put it in the global key buf
*   also checks if we need to update flags and calls the formatter to format it
*/
void do_keyboard() {
    kbdus[ctrl_release]=ctrl_release;
    kbdus[alt_release]=alt_release;
    kbdus[caps_release]=caps_release;
    kbdus[shift_release]=shift_release;
     kbdus[shift_release_right]=shift_release;
/* stop interrupts */
	cli();
	
/* translate scancode to ascii in order to echo key */
	uint32_t scancode = inb (KEYBOARD_DATA);


       if ((scancode==shift_release_right)||(scancode==ctrl_release)||(scancode==caps_val)||(scancode==shift_release)||(scancode==alt_release)||(!(scancode & KEYBOARD_TOPB)))
    {
        switch(kbdus[scancode]) {
        case alt_val:
            alt_flag=1;
            break;
        case alt_release:
            alt_flag=0;
            break;
        case caps_val:
        if(caps_flag==0){
            caps_flag=1;}
        else{caps_flag=0;}
        
            break;
        case caps_release:
            
            break;
        case ctrl_val:
            ctrl_flag=1;
            break;
        case ctrl_release:
            ctrl_flag=0;
            break;
        case shift_val:
            shift_flag=1;
            break;
        case shift_release:
            shift_flag=0;
            break; 
        case shift_release_right:
            shift_flag=0;
            break; 
        default:
         global_key=(kbdus[scancode]);
        global_key=key_formatter(global_key);           
       
         
    }
}
/* send eoi to keyboard line on pic and enable interrupts */
	send_eoi(KEYBOARD_IRQ);
	sti();

}
/*127 max buffer size starts array from 0 so 128 values total*/
int32_t max_bytes=max_buff_size;

/*
* uint8_t key_formatter(uint8_t keyval);
*   Inputs: unsigned character to be processes
*   Return Value: the processed value
*   Function: checks flags and sees what modifers should be applied
*/
uint8_t
key_formatter(uint8_t keyval){
    if (keyval==nall) {
        return nall;
    }
    if (keyval==ll&&ctrl_flag)
    {
        /*SET RESET KEY FLAG TO TRUE*/
        rese_key=1;
        putc_key(aa,rese_key,backs_key);
        rese_key=0;
        key_index=0;
        /*these zeros are hardcoded such that they are all false so we dont dleete or reset*/
        putc_key('[',0,0);
        putc_key('m',0,0);
        putc_key('a',0,0);
        putc_key('g',0,0);
        putc_key('a',0,0);
        putc_key(']',0,0);
        putc_key('$',0,0);
        putc_key(' ',0,0);
        return nall;
    }
    else if (caps_flag&&!shift_flag) {
        if (keyval>=aa&&keyval<=zz) {
            keyval=keyval-alpha_off;
        }
    }
    else if (!caps_flag&&shift_flag) {
        if (keyval>=aa&&keyval<=zz) {
            keyval=keyval-alpha_off;
        }
    }
    if (keyval==backs) {
        if(key_index!=0){key_index--;
        backs_key=1; /*set flag to 1*/
        }
        else{
            return nall;
        }
        full=0; /*if we delete our buffer would no longer be full*/
        return ' ';/*return the space character so our read function echos "blank or nothing"*/
    }

    if (key_index==max_bytes&&keyval==newline&&!full) {
        
        full_new=1;
        full=1;
        return keyval;
    }
    if (key_index<max_bytes&&keyval!=newline&&!full) {
        full=0;
        return keyval;
    }
    if (keyval==newline&&!full) {
        full_new=1;
        full=1;
        return keyval;
    }
    return nall;
}





/*
* int32_t t_read(int32_t fd,unsigned char * buff,int32_t num_bytes);
*   Inputs: the fd and a buffer to put values into as well as the number of bytes to write
*   Return Value: the buffer with all key presses
*   Function: takes keys from the global key variable and puts them in the buffer if appropriate
*/
int32_t t_read(int32_t fd,unsigned char * buff,int32_t num_bytes){
    int32_t i;
    for (i = 0; i < max_buff_size; ++i)
    {
        buff[i]=nall;
    }
    max_bytes=num_bytes-1; /*subtract 1 so we can use<= not just less than*/
    putc_key('[',0,0); /*print [maga]$ to the screen so it looks nice*/
    putc_key('m',0,0); /*use my special putc function with flags set to 0*/
    putc_key('a',0,0); /*this means we dont treat it as a reset or backspace*/
    putc_key('g',0,0);
    putc_key('a',0,0);
    putc_key(']',0,0);
    putc_key('$',0,0);
    putc_key(' ',0,0);
/*reset flags*/

    key_index=0;
    full=0;
    full_new=0;
   global_key=nall;
   /*while we havent seen a newline we check that there is no newline in the buffer or certain other flags (all for newline)*/
    while (global_key!=newline&&buff[key_index-1]!=newline&&!full_new) {
        cli(); /*this seems to make my keyboard more efficent honestly*/
        key_spam_flag=1;
        if(global_key!=nall){
            /*if its nall this means our formatter said we shouldnt print it*/
             if (!backs_key){
            buff[key_index]=global_key;
            putc_key(global_key,rese_key,backs_key);
            full_new=0;
            global_key=nall;
            key_index++;
            }
            else{
            buff[key_index]=nall;
            putc_key(global_key,rese_key,backs_key);
            full_new=0;
            global_key=nall;
         

            }
        }
         backs_key=0; /*reset flags*/
        rese_key=0;
        key_spam_flag=0;
       sti();
       

        
    }
    if(buff[key_index-1]!=newline||full_new) { /*make sure that the new line was inserted to our buffer*/
    buff[key_index]=newline;
    putc_key(newline,0,0);
}
    return key_index+1; /*return how many bytes we wrote*/
}

/*
* int32_t t_write(int32_t fd,unsigned char * buff,int32_t num_bytes);
*   Inputs: the fd and a buffer to put values into as well as the number of bytes to write
*   Return Value: the number of bytes not written
*   Function: takes keys from the buffer and puts them in the buffer if appropriate
*/
int32_t t_write(int32_t fd,unsigned char * buff,int32_t num_bytes){
    int32_t i=0; /*write out the buffer while its not null or we reach num bytes*/
    while (buff[i]!=nall&&i!=num_bytes) {
        putc_key(buff[i],0,0);
        i++;
    }
    return i; /*should return 0 if we read all the bytes*/
}

/*
* int32_t t_oepn(const uint8_t filename);
*   Inputs: the filename
*   Return Value: returns 0 (doesnt need to return -1 yet that comes later)
*   Function: resets flags and clears the screen
*/
int32_t t_open(const uint8_t filename){
/*reset flags and clear the screen*/
    rese_key=0;
    clear();
    key_spam_flag=0;
    key_index=0;
    full=0;
    full_new=0;
    return 0;
}

/*
* int32_t t_write(int32_t fd);
*   Inputs: the file descriptor
*   Return Value: 0 if it succes -1 if it fails
*   Function: resets the flags (will do other things later)
*/
int32_t t_close(int32_t fd){
/*reset flags*/
    key_spam_flag=0;
    key_index=0;
     full=0;
    full_new=0;
    return 0;   
}

/*should i stop writing on newline
stop reading on num byes?

what about cursor

*/
