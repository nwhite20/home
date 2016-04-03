#include "rtc.h"


#define default_speed 2
#define bitformask 0x1
#define sizeofint 32

/*
* void rtc_init();
*   Inputs: none
*   Return Value: none
*   Function: Initializes rtc, sets rate and unmasks rtc interrupt line
*/
int32_t refreshrate;
void rtc_init() {
	
	/* initialization for RTC */

	outb(REGISTER_B, RTC_PORT);
	char curr = inb(RTC_DATA);
	outb(REGISTER_B, RTC_PORT);
	outb(curr | RTCMASK1, RTC_DATA);

	/* rate set such that the interrupt occurs at 2 Hz */
	char rate = refreshrate;

	outb(REGISTER_A, RTC_PORT); 
	curr = inb(RTC_DATA);
	outb(REGISTER_A, RTC_PORT);
	outb((curr & RTCMASK2)|rate, RTC_DATA);

	/* unmasking rtc interrupt line */
	enable_irq(RTC_IRQ);

}
volatile int rtc_flag=0;
/*
* void do_rtc();
*   Inputs: none
*   Return Value: none
*   Function: Processes an rtc interrupt
*/
void do_rtc() {

	/* stop interrupts */
	cli();

	/* read in trash to get next interrupt */
	outb(REGISTER_C, RTC_PORT);
	inb(RTC_DATA);	
	rtc_flag=1;
	/* flicker screen - COMMENT OUT TO TEST KEYBOARD */
	/*test_interrupts();*/

	/* send eoi and resume interrupts */
	send_eoi(RTC_IRQ);
	sti();

}

/*
* uint32_t num) is_power(uint32_t num));
*   Inputs: a number
*   Return Value: 0 if its not a power of 2 1 if it is a power of 2
*   Function: bitshifts to check if there is only ONE 1 in the bitfield meaning its a power of 2
*/
uint32_t is_power(uint32_t num){
uint32_t bitmask=bitformask;
uint32_t p=0;
uint32_t i;
	for (i = 0; i < sizeofint; ++i)
	{
		if(num&bitmask){
			p++;
		}
		bitmask=bitmask<<bitformask;
	}
	if(p!=1){
		return 0;
	}
	else{
		return 1;
	}
}

/*
* int32_t prock (uint32_t processee);
*   Inputs: an unsigned integer
*   Return Value: the value to set our rtc refresh rate to. Returns -1 if its too big
*   Function: switch statements to check what to set our refresh rate to.
*/
int32_t prock (uint32_t processee) { 
	/*frequency =  32768 >> (rate-1); EQUATION FROM OS DEV*/
	switch (processee){
		case 2: /*if the user tries to set the refresh rate to 2*/
			return 15; /*the math from os dev tells us we should pass in 15 to the rtc refresh value*/
			break;
		case 4: /*if the refresh rate is 4 then we set it to that value*/
			return 14;
			break; /****these breaks are unessacarry but were left in for debugging***/
		case 8: /*8 is 13*/
			return 13;
			break;
		case 16:
			return 12;
			break;
		case 32:
			return 11;
			break;
		case 64: 	/*semi medium refresh rate comment filler pretty much*/
			return 10;
			break;
		case 128:
			return 9;
			break;
		case 256:
			return 8;
			break;
		case 512:
			return 7;
			break;
		case 1024: /*1024 is the maximum refresh rate which corresponds to 6*/
			return 6;
			break;
		default: /*return -1 if the value is too large as per the documentation or not a power of 2*/
			return -1;	
		}
}

/*
* int32_t rtc_read(int32_t fd,void * buff,int32_t num_bytes);
*   Inputs: a file descriptor a buffer thats useless and num bytes which is useless
*   Return Value: 0 on return
*   Function: Waits until an rtc interrupt occurs to return.
*/
int32_t rtc_read(int32_t fd,void * buff,int32_t num_bytes){
    
    rtc_flag=0;
    while(!rtc_flag){
    }

   /* int i=0;
   while (i!=refreshrate) {
    	if(rtc_flag==1) {
    		i++;
    		rtc_flag=0;
    	}
    } */
    return 0;
}

/*
* int32_t rtc_write(int32_t fd,void * buff,int32_t num_bytes);
*   Inputs: a file descriptor a buffer that contains the new refresh rate and num bytes which is 4 since its an int
*   Return Value: 0 if sucessfull -1 if failed (not a power of 2 or too fast)
*   Function: Waits until an rtc interrupt occurs to return.
*/
int32_t rtc_write(int32_t fd,int32_t buff,int32_t num_bytes){
	if (!is_power((unsigned int)buff))
	{
		return -1;
	}
    refreshrate=prock((int)buff)/*/(int32_t)TWOHZRATE*/;
    if (refreshrate==-1)
    {
    	return refreshrate;
    }
    rtc_init();
    return 0;
}

/*
* int32_t rtc_open(const uint8_t filename)
*   Inputs: a file name
*   Return Value: 0 
*   Function: sets the default rtc rate to 2 hz
*/
int32_t rtc_open(const uint8_t filename){
    refreshrate=prock(default_speed)/*/(int32_t)TWOHZRATE*/;
    rtc_init();
    return 0;
    
}

/*
* int32_t rtc_close(int32_t fd)
*   Inputs: a file descriptor
*   Return Value: 0 
*   Function: restores the default rtc rate
*/
int32_t rtc_close(int32_t fd){
    refreshrate=prock(default_speed)/*/(int32_t)TWOHZRATE*/;
    rtc_init();
    return 0;
}

/*ask about rtc */

