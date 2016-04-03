#ifndef _INTERRUPTS_H
#define _INTERRUPTS_H

#include "lib.h"
#include "x86_desc.h"
#include "types.h"
#include "IDT.h"
#include "keyboard.h"
#include "rtc.h"
#include "i8259.h"

#define TIMER_VEC 0x20
#define KEYBD_VEC 0x21
#define RTC_VEC   0x28
#define SYSCALL_VEC	 0x80

void setup_idt();

void do_timer();

void do_syscall();

void do_general_int();

/* EXCEPTION HANDLERS */
void do_divide_error();
void do_debug();
void do_nmi();
void do_int3();
void do_overflow();
void do_bounds();
void do_invalid_op();
void do_device_not_available();
void do_doublefault_fn();
void do_coprocessor_segment_overrun();
void do_invalid_TSS();
void do_segment_not_present();
void do_stack_segment();
void do_general_protection();
void do_page_fault();
void do_coprocessor_error();
void do_alignment_check();
void do_machine_check();
void do_simd_coprocessor_error();

#endif /* _INTERRUPTS_H */
