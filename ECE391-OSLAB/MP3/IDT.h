#ifndef _IDT_H
#define _IDT_H

#ifndef ASM

#include "types.h"
#include "interrupts.h"
#include "keyboard.h"
#include "rtc.h"

#define NUM_EXCEPTIONS 20

/* declaring handlers */
extern int* exceptions[NUM_EXCEPTIONS];
extern void general_int();
extern void syscall();
extern void timer();
extern void keyboard();
extern void rtc();

#endif /* ASM */

#endif /* _IDT_H */
