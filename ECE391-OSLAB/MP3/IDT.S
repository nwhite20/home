#define ASM     1

.text

# making handler addresses available
.globl  exceptions
.globl  syscall
.globl  timer
.globl  rtc
.globl  keyboard
.globl  general_int

# placeholder system call handler
syscall:

	pusha
	call	do_syscall
	popa

	
	ret
	
# unused timer interrupt handler
timer:


	
	pusha	
	call	do_timer
	popa

	
	iret

# keyboard interrupt handler
# void keyboard()
#   Inputs: none
#   Return Value: none
#	Function: Passes control to keyboard exception handler and irets after
#
keyboard:

	

	pusha	
	call	do_keyboard
	popa

	
	iret

# rtc interrupt handler
# void rtc()
#   Inputs: none
#   Return Value: none
#	Function: Passes control to rtc exception handler and irets after
#

rtc:

	

	pusha	
	call	do_rtc
	popa

	
	iret

# ALL EXCEPTIONS HANDLERS BELOW FOLLOW THE FOLLWING STRUCTURE
#	YES WE CAN COPY PASTE FOR OUR OWN SANITY WE DID NOT MAKES THE FILE SHORTER AND CLEANER
# EACH CALLS A C FUNCTION THAT PRINTS THE EXCEPTION AND SPINS IN WHILE(1)

# GENERAL EXCEPTION HANDLER INTERFACE
# void handler_name()
#   Inputs: none
#   Return Value: none
#	Function: Passes control to appropriate C exception handler
#

divide_error:
#called for divide erros
	pusha
	call	do_divide_error
	popa	
	ret

debug:	
#called for debugging problems
	pusha
	call	do_debug
	popa	
	ret

nmi:
#called for nmi
	pusha
	call	do_nmi
	popa	
	ret

int3:
#called for int3
	pusha
	call	do_int3
	popa
	ret

overflow:
#called for overflow shstuff
	pusha
	call	do_overflow
	popa
	ret

bounds:
#called for bounds checking
	pusha
	call	do_bounds
	popa
	ret

invalid_op:
#called for invalid operands
	pusha
	call	do_invalid_op
	popa
	ret

device_not_available:
#called for device not aval
	pusha
	call	do_device_not_available
	popa
	ret

doublefault_fn:
#called foR GGG DOUBKLE FAULLLTLTL
	pusha
	call	do_doublefault_fn
	popa	
	ret

coprocessor_segment_overrun:
#called for segment overruns
	pusha
	call	do_coprocessor_segment_overrun
	popa
	ret

invalid_TSS:
#called for invalid_TSS
	pusha
	call	do_invalid_TSS
	popa
	ret

segment_not_present:
#called for segment not present
	pusha
	call	do_segment_not_present
	popa
	ret

stack_segment:
#called for stack segment
	pusha
	call	do_stack_segment
	popa
	ret

general_protection:
#called general protection when nothing looks right
	pusha
	call	do_general_protection
	popa
	ret

page_fault:
#called for page faults
	pusha
	call	do_page_fault
	popa
	ret

coprocessor_error:
#called upon coprocessor error
	pusha
	call	do_coprocessor_error
	popa
	ret

alignment_check:
#called to check alignment
	pusha
	call	do_alignment_check
	popa
	ret

machine_check:
#called for machine check
	pusha
	call	do_machine_check
	popa
	ret

simd_coprocessor_error:
#called upon coprocessor error
	pusha
	call	do_simd_coprocessor_error
	popa
	ret

# GENERAL INTERRUPT - FILLED INTO ALL UNUSED IDT ENTRIES AT INITIALIZATION - SHOULD NOT GET
general_int:

	pusha
	call	general_int
	popa
	ret

#  ARRAY OF EXCEPTION HANDLER ADDRESSES
exceptions:

.long divide_error, debug, nmi, int3, overflow, bounds, invalid_op, device_not_available, doublefault_fn, coprocessor_segment_overrun, invalid_TSS, segment_not_present, stack_segment, general_protection, page_fault, 0, coprocessor_error, alignment_check, machine_check, simd_coprocessor_error

