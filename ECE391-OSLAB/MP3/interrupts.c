#include "interrupts.h"

/*
* void setup_idt();
*   Inputs: none
*   Return Value: none
*   Function: Fills out IDT with initial values (currently includes setting keyboard and rtc handlers)
*/
void setup_idt() {

/* declaring and filling out an IDT entry struct */
/* bits set appropriately for exceptions, system calls and other interrupts */
	int i;
	idt_desc_t idt_entry;
	idt_entry.reserved4 = 0;
	idt_entry.reserved3 = 0;
	idt_entry.reserved2 = 1;
	idt_entry.reserved1 = 1;
	idt_entry.reserved0 = 0;
	idt_entry.size = 1;
	idt_entry.present = 1;
	idt_entry.dpl = 0;
	idt_entry.seg_selector = KERNEL_CS;

/* filling out exceptions */
	for (i=0;i<NUM_EXCEPTIONS;i++) {

		if (exceptions[i]) {
		SET_IDT_ENTRY(idt_entry, exceptions[i]);
		idt[i] = idt_entry;
		}

	}

/* changing reserved3 bit for keyboard and rtc IDT entries */
	idt_entry.reserved3 = 1;

	//idt[TIMER_VEC] = idt_entry; unused until timer interrupt implemented

/* filling handler addresses for keyboard and rtc */
	SET_IDT_ENTRY(idt_entry, keyboard);
	idt[KEYBD_VEC] = idt_entry;

	SET_IDT_ENTRY(idt_entry, rtc);
	idt[RTC_VEC] = idt_entry;

	idt_entry.dpl = 3;
	idt_entry.seg_selector = USER_CS;
	SET_IDT_ENTRY(idt_entry, syscall);
	idt[SYSCALL_VEC] = idt_entry;
	
/* fill all empty IDT entries with a general interrupt handler */
	for (i=NUM_EXCEPTIONS+1;i<NUM_VEC;i++) {

		if ((!idt[i].val[0])&&(!idt[i].val[1])) {
		SET_IDT_ENTRY(idt_entry, general_int);
		idt[i] = idt_entry;
		}

	}

}

/* DEVICE HANDLERS */
void do_timer() {

	/* currently unused */

}

/* placeholder function for system call vector */
void do_syscall() {

	printf("SYSTEM CALL PLACEHOLDER");

}

/* filled into all unused IDT entries by default
* 	void do_general_int();
*   Inputs: none
*   Return Value: none
*   Function: Prints that a general interrupt has been reached and asserts that this is undesirable
*/
void do_general_int() {

	printf("GENERAL INTERRUPT (BAD)");

}

/* EXCEPTION HANDLERS ALL FOLLOW THIS FOLLOWING INTERFACE*/

/* GENERAL EXCEPTION HANDLER INTERFACE
* void do_handler_name();
*   Inputs: none
*   Return Value: none
*   Function: Prints exception name and loops with while(1) (for now)
*/

void do_divide_error() {

	printf("DIVIDE BY ZERO ERROR");
	while(1){}

}

void do_debug() {

	printf("DEBUG EXCEPTION");
	while(1){}
	
}

void do_nmi() {

	printf("THIS ISN'T USED");
	while(1){}

}

void do_int3() {

	printf("IT'S A BREAKPOINT");
	while(1){}
	
}

void do_overflow() {

	printf("OVERFLOW ERROR");
	while(1){}
	
}

void do_bounds() {

	printf("OUT OF BOUNDS ERROR");
	while(1){}
	
}

void do_invalid_op() {

	printf("INVALID OPCODE ERROR");
	while(1){}
	
}

void do_device_not_available() {

	printf("DEVICE NOT AVAILABLE ERROR");
	while(1){}
	
}

void do_doublefault_fn() {

	printf("GG DOUBLE FAULT");
	while(1){}
	
}

void do_coprocessor_segment_overrun() {

	printf("COPROCESSOR OVERRUN ERROR");
	while(1){}
	
}

void do_invalid_TSS() {

	printf("INVALID TSS");
	while(1){}
	
}

void do_segment_not_present() {

	printf("SEGMENT NOT PRESENT ERROR");
	while(1){}
	
}

void do_stack_segment() {

	printf("STACK SEGMENT FAULT ERROR");
	while(1){}
}

void do_general_protection() {

	printf("GENERAL PROTECTION FAULT");
	while(1){}
	
}

void do_page_fault() {

	printf("PAGE FAULT");
	while(1){}
	
}

void do_coprocessor_error() {

	printf("FLOATING POINT ERROR");
	while(1){}
	
}

void do_alignment_check() {

	printf("ALIGNMENT CHECK ERROR");
	while(1){}
	
}

void do_machine_check() {

	printf("MACHINE CHECK ERROR");
	while(1){}
	
}

void do_simd_coprocessor_error() {

	printf("SIMD FLOATING POINT EXCEPTION");
	while(1){}
	
}

