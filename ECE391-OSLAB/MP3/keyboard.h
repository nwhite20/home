#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "interrupts.h"
/*size of keyboard data*/
#define KEYBOARD_IRQ 1
#define KEYBOARD_DATA 0x60
#define KEYBOARD_TOPB 0x80
#define NUM_SCANCODES 128


/*init function*/
void keyboard_init();

/* PIC INTERRUPT */
void do_keyboard();


unsigned char
key_formatter(unsigned char keyval);


int32_t t_read(int32_t fd,unsigned char * buff,int32_t num_bytes);
int32_t t_write(int32_t fd,unsigned char * buff,int32_t num_bytes);
int32_t t_open(const uint8_t filename);
int32_t t_close(int32_t fd);

#endif /* _KEYBOARD_H */
