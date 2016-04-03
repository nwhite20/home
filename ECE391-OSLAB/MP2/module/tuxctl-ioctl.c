/* tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"

#define debug(str, ...) \
	printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)

/************************ Protocol Implementation *************************/

/* tuxctl_handle_packet()
 * IMPORTANT : Read the header for tuxctl_ldisc_data_callback() in 
 * tuxctl-ld.c. It calls this function, so all warnings there apply 
 * here as well.
 */

#define fill 0;

int tuxctl_ioctl_init(struct tty_struct* tty);
int tuxctl_ioctl_buttons(struct tty_struct* tty, unsigned int* arg);
int tuxctl_ioctl_set_led(struct tty_struct* tty, unsigned int arg);
void tuxctl_bioc(unsigned b, unsigned c);

// static unsigned char ledSettings[4] = {0};
static unsigned int buttonSettings = 0;
spinlock_t buttonLock;

// LEGENDARY LED
const static unsigned char led_display_info [16] = {0xE7, 0x06, 0xCB, 0x8F, 0x2E, 0xAD, 
	0xED, 0x86, 0xEF, 0xAF, 0xEE, 0x6D, 0xE1, 0x4F, 0xE9, 0xE8};

void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* packet)
{

    unsigned a, b, c;

    a = packet[0]; /* Avoid printk() sign extending the 8-bit */
    b = packet[1]; /* values when printing them. */
    c = packet[2];

    // printk("packet : %x %x %x\n", a, b, c); 

    switch(a) {

    case MTCP_RESET:
    	return (void) tuxctl_ioctl_init(tty);
    
    case MTCP_BIOC_EVENT:
    		tuxctl_bioc(b, c); // saving current button status
    	return;

    case MTCP_CLK_EVENT:
    	return;

    case MTCP_ACK:
    	return;

    default:
    	return;
    }

    // tuxctl_ioctl (tty, file, a, arg)

}


void
tuxctl_bioc (unsigned b, unsigned c)
{
	// initialization
	unsigned long flags; // set up the lock
	spin_lock_irqsave (&buttonLock, flags);

	// clear high bits
	b = b & 0x0F;
	c = c & 0x0F;

	// bit manipulation to get the format we want
	c = ((c & 0x09) | (((c & 0x04) >> 1) | ((c & 0x02) << 1))) << 4;
	
	// make the right buttons
	buttonSettings = (~(b | c)) & 0xFF;
	spin_unlock_irqrestore (&buttonLock, flags);
}

/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS packet and the time the 3-byte ACK packet finishes         *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/
int 
tuxctl_ioctl (struct tty_struct* tty, struct file* file, 
	      unsigned cmd, unsigned long arg)
{
    switch (cmd) {

	case TUX_INIT:

		return tuxctl_ioctl_init(tty);

	case TUX_BUTTONS:

		return tuxctl_ioctl_buttons(tty, (unsigned int*) arg);

	case TUX_SET_LED:

		return tuxctl_ioctl_set_led(tty, arg);

/*
	case TUX_LED_ACK:

		return tuxctl_ioctl__ack();

	case TUX_LED_REQUEST:

		return tuxctl_ioctl_request();

	case TUX_READ_LED:

		return tuxctl_ioctl_read(); */

	default:

	    return -EINVAL;
    }
}


/* tuxctl_ioctl_buttons()
 *   INPUTS: Integer pointer to fill with buttons
 *   OUTPUTS: none
 *   RETURN VALUE: Returns currently pressed buttons
 *   SIDE EFFECTS: Takes care of the button call
 */
int 
tuxctl_ioctl_buttons(struct tty_struct* tty, unsigned int* arg)
{
	// initialization
	unsigned long flags; // set up the lock
	int error = 0;

	// give to user 

	spin_lock_irqsave (&buttonLock, flags);

	error = copy_to_user((void *)arg, (void *)&buttonSettings, sizeof(int));

	spin_unlock_irqrestore (&buttonLock, flags);

	// ERROR REPORT IF ERROR
	if (error != 0) {
		return -EINVAL;
	}
	return 0;
}


/* tuxctl_ioctl_init()
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Initializes the tux controller
 */
int 
tuxctl_ioctl_init(struct tty_struct* tty)
{
	unsigned char message[2];

	// sending message to turn on BIOC and USR LED mode
	// also reinitializing

	message[0] = MTCP_BIOC_ON;
	message[1] = MTCP_LED_USR;

	buttonSettings = 0;
	buttonLock = SPIN_LOCK_UNLOCKED;

	tuxctl_ldisc_put(tty,&message[0],1);
	tuxctl_ldisc_put(tty,&message[1],1);

    return 0;
}


/* tuxctl_ioctl_set_led()
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Sets the led
 */
int 
tuxctl_ioctl_set_led(struct tty_struct* tty, unsigned int arg)
{
	// necessary variables
	unsigned int display = 0xFFFF & arg;
	unsigned int onmask = 0x0F0000 >> 16;
	unsigned int dpmask = 0x0F000000 >> 24;
	unsigned char output;
	int i;

// sending messages to set LEDs
	output = MTCP_LED_SET;
	tuxctl_ldisc_put(tty,&output,1);
	output = onmask;
	tuxctl_ldisc_put(tty,&output,1);

//iterating through outputting for each necessary led
	for (i = 0; i < 4; i ++) {
		output = 0;
		if (onmask&1) {
			output = led_display_info[display] + ((onmask & dpmask) & 1);
			tuxctl_ldisc_put(tty,&output,1);
		}

// shifting right
		onmask = onmask >> 1;
		dpmask = dpmask >> 1;
		}

	

    return 0;
}


