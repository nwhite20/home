
.data					# section declaration

        # Useful offset constants for accessing members of a 
        # struct mp1_blink_struct structure
        LOCATION   = 0    
        ON_CHAR    = 2
        OFF_CHAR   = 3 
        ON_LENGTH  = 4
        OFF_LENGTH = 6
        COUNTDOWN  = 8
        STATUS     = 10
        NEXT       = 12
        STRUCT_SIZE = 16

        LOCMAX = 2000

# Pointer to head of list (initialized to NULL)
mp1_list_head:
        .long   0

.text					# section declaration

# Export the function symbol names

.global mp1_rtc_tasklet
.global mp1_ioctl



# void mp1_poke(void);
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %cl  - The byte you wish to write
#            %eax - Offset from the start of video memory that you wish
#                   to write to
#   Outputs: Text-mode video screen is written to at location %eax with
#            the byte in %cl
# Registers: Clobbers EDX

mp1_poke:
        
	movl    vmem_base_addr(,1),%edx
	movb    %cl,(%edx,%eax,1)
	ret

# void mp1 rtc tasklet (unsigned long);
#
# Called on RTC interrupt
#
# Interface: C-style
#    Inputs: None
#   Outputs: None
# Registers: Saves callee-save

mp1_rtc_tasklet:

        pushl   %ebp # new stack frame
        movl    %esp,%ebp

        pushl   %ebx # callee save
        pushl   %esi
        pushl   %edi

        movl   mp1_list_head,%ebx # getting list head
        cmpl   $0,%ebx  # checking if no list head and ending if so
        je     taskend

taskloop:

        xorl   %ecx,%ecx # clearing registers
        xorl   %eax,%eax
        xorl   %edx,%edx

        movw   COUNTDOWN(%ebx),%dx # loading countdown
        decw   %dx
        movw   %dx,COUNTDOWN(%ebx) # updating counting
        cmpl   $0,%edx
        jg     not_down # skipping if not 0 count

        movw   LOCATION(%ebx),%ax
        movw   STATUS(%ebx),%dx # status check and switch
        cmpw   $0,%dx
        je     zero

        movw   OFF_LENGTH(%ebx),%cx # updating struct values
        movw   %cx,COUNTDOWN(%ebx) # for status value of on to off
        xorl   %ecx,%ecx
        movb   OFF_CHAR(%ebx),%cl
        decw   %dx

        jmp    task_poke

zero:
        
        movw   ON_LENGTH(%ebx),%cx # updating struct values
        movw   %cx,COUNTDOWN(%ebx) # for status value of on to off
        xorl   %ecx,%ecx
        movb   ON_CHAR(%ebx),%cl
        incw   %dx

task_poke:
        movw   %dx,STATUS(%ebx) # poking appropriate character
        addw   %ax,%ax
        call   mp1_poke

not_down:

        movl   NEXT(%ebx),%ebx # looping through list
        cmpl   $0,%ebx
        jne    taskloop

taskend:

        popl   %edi
        popl   %esi
        popl   %ebx

        leave
        ret

# int mp1_ioctl (unsigned long arg, unsigned long cmd);
#
# Dispatches to function
#
#    Inputs: unsigned long arg to function, unsigned long cmd to choose which function
#   Outputs: -1 on error
# Registers: Saves callee-save

mp1_ioctl:
        movl    4(%esp),%edx # retrieving arguments passed to dispatcher 
        movl    8(%esp),%eax 

        movl    $0,%ecx # starting counter

cmdloop: 
        cmpl    $0,%eax # testing for command
        jne     notfound 

        jmp     *jumptable(,%ecx,4) # jumping based on counter to appropriate command

notfound:
        decl    %eax # decrementing command argument to check
                
        incl    %ecx # increasing counter
        cmpl    $4,%ecx # checking if out of command range
        je      error # command argument outside of 0-3
        jmp     cmdloop

error:  movl    $-1,%eax # returning -1 for error  

	ret


# int mp1_ioctl_add(unsigned long arg);
#
# Adds a new blink structure
#
# Interface: C-style
#    Inputs: unsigned long arg which is a pointer to a user-space structure
#   Outputs: -1 if error, 0 if successful add
# Registers: Saves callee-save

mp1_ioctl_add:

        pushl   %ebp # new stack frame
        movl    %esp,%ebp

        pushl   %ebx
        pushl   %esi
        pushl   %edi


        movl    $0,%edx
        movl    8(%ebp),%edx # getting argument

        cmpl    $0,%edx
        je      mlc_error
        movl    $STRUCT_SIZE,%ecx
        pushl   %ecx
        movl    %edx,%esi
        call    mp1_malloc # using mp1 malloc to make space for new struct
        movl    %esi,%edx
        popl    %ecx

        cmpl    $0,%eax # checking for malloc error
        je      mlc_error
        
        pushl   %ecx
        pushl   %edx
        pushl   %eax
        movl    %eax,%esi
        call    mp1_copy_from_user # pushed arguments and called copy_from_user to copy struct
        addl    $12,%esp # popping arguments

        cmpl    $0,%eax # checking copy return for error
        jne     copy_error

        movw    LOCATION(%esi),%dx # error catching for location
        cmpw    $0,%dx
        jl      copy_error
        movw    $LOCMAX,%ax
        cmpw    %ax,%dx
        jge     copy_error

        movw    ON_LENGTH(%esi),%dx
        movw    %dx,COUNTDOWN(%esi)

        movw    $1,STATUS(%esi)

        movl    mp1_list_head,%edx # getting current list head
        
        cmpl    $0,%edx # checking for null pointer
        jne     hashead # jumping if list exists
        movl    $0,NEXT(%esi) # setting next pointer to null and adding as head
        movl    %esi,mp1_list_head
        jmp     headmoved

hashead:
        movl    %edx,NEXT(%esi) # moving current head to new head's next
        movl    %esi,mp1_list_head # setting new head

headmoved:
        xorl    $0,%ecx # clearing
        xorl    $0,%eax

        movb    ON_CHAR(%esi),%cl # calling poke with correcty struct arguments
        movw    LOCATION(%esi),%ax
        addw    %ax,%ax
        call    mp1_poke

        movl    $0,%eax

        popl   %edi
        popl   %esi
        popl   %ebx

        leave 
	ret
        
copy_error:
        pushl   %esi
        call    mp1_free # freeing allocated memory on error
        addl    $4,%esp    
mlc_error:
        movl    $-1,%eax

        popl   %edi
        popl   %esi
        popl   %ebx

        leave
        ret

# int mp1_ioctl_remove(unsigned long arg);
#
#  Removes a blink structure
#
# Interface: C-style
#    Inputs: unsigned long arg which is a location
#   Outputs: -1 if error, 0 if successful add
# Registers: Saves callee-save

mp1_ioctl_remove:

        pushl   %ebp # new stack frame
        movl    %esp,%ebp

        pushl   %ebx # callee save
        pushl   %esi
        pushl   %edi

        xorl    %esi,%esi

        movl    $0,%edx
        movl    8(%ebp),%edx # getting argument

        movl    mp1_list_head,%ebx # getting list head to start searching
        cmpl    $0,%ebx
        je      remove_error # error is no list

        cmpw    LOCATION(%ebx),%dx # checking if head is desired element
        je      rmvhead

        movl    NEXT(%ebx),%esi # if head isn't desired element and next is null, error
        cmpl    $0,%esi
        je      remove_error

rmvtop: 
        cmpw    LOCATION(%esi),%dx # checking location of current
        je      rmvother

        movl    %esi,%ebx # moving and checking if at end of list
        movl    NEXT(%esi),%esi
        cmpl    $0,%esi
        jne     rmvtop
        jmp     remove_error

rmvother:

        movl    NEXT(%esi),%edi # removing an item that isn't the head
        movl    %edi,NEXT(%ebx)
        jmp     rmvfree

rmvhead: 
        
        movl    NEXT(%ebx),%edi # removing head
        movl    %edi,mp1_list_head
        movl    %ebx,%esi

rmvfree:

        pushl   %esi
        call    mp1_free
        addl    $4,%esp
        movl    $0,%eax
        jmp     remove_end

remove_error:

        movl    $-1,%eax

remove_end:
        popl    %edi # restoring callee save
        popl    %esi
        popl    %ebx



        leave 

        ret
        

# int mp1_ioctl_find(unsigned long arg);
#
# Interface: C-style
#    Inputs: unsigned long arg that is a pointer to a blink struct
#   Outputs: -1 if error, 0 if successful find
# Registers: Saves callee-save

mp1_ioctl_find:

        pushl   %ebp # new stack frame
        movl    %esp,%ebp

        pushl   %ebx # callee save
        pushl   %esi
        pushl   %edi

        movl    $0,%edx
        movl    8(%ebp),%edx # getting argument
        cmpl    $0,%edx
        je      find_error
        xorl    %eax,%eax
        movw    LOCATION(%edx),%ax # extracting location
        movl    mp1_list_head,%ebx

find_loop:
        cmpl    $0,%ebx # checking of at end of list
        je      find_error

        cmpw    LOCATION(%ebx),%ax # comparing locations
        je      found

        movl    NEXT(%ebx),%ebx # continuing through list if not found
        jmp     find_loop

found: 
        
        movl    $STRUCT_SIZE,%ecx
        pushl   %ecx
        pushl   %ebx
        pushl   %edx
        call    mp1_copy_to_user # pushed arguments and called copy_to_user to copy struct
        addl    $12,%esp # popping arguments
        cmpl    $0,%eax # checking copy return for error
        jne     find_error
        movl    $0,%eax
        jmp     find_end

find_error:
        movl    $-1,%eax

find_end:
        popl    %edi # restoring callee save
        popl    %esi
        popl    %ebx

        leave
	ret


# int mp1_ioctl_sync(unsigned long arg);
#
# Syncs two blink structures
#
# Interface: C-style
#    Inputs: unsigned long arg which holds two locations
#   Outputs: -1 if error, 0 if successful add
# Registers: Saves callee-save


mp1_ioctl_sync:

        pushl   %ebp # new stack frame
        movl    %esp,%ebp

        pushl   %ebx # callee save
        pushl   %esi
        pushl   %edi

        xorl    %eax,%eax # getting argument portions into registers
        xorl    %edx,%edx
        movw    8(%ebp),%ax
        movw    10(%ebp),%dx

        movl    mp1_list_head,%ebx

syncfirst: 
        cmpl    $0,%ebx
        je      sync_error

        cmpw    LOCATION(%ebx),%ax # comparing locations
        je      foundfirst

        movl    NEXT(%ebx),%ebx # continuing throug list if not same
        jmp     syncfirst


foundfirst:
        movl    %ebx,%esi # saving first matching location pointer
        movl    mp1_list_head,%ebx

syncsecond:
        cmpl    $0,%ebx # continuing by searching for second
        je      sync_error

        cmpw    LOCATION(%ebx),%dx # searching for matching location
        je      foundsecond

        movl    NEXT(%ebx),%ebx # moving through list
        jmp     syncsecond

foundsecond:

        xorl    %ecx,%ecx # clearing
        xorl    %eax,%eax
        movw    ON_LENGTH(%ebx),%ax # copying sync traits to target
        movw    %ax,ON_LENGTH(%esi)

        movw    OFF_LENGTH(%ebx),%ax
        movw    %ax,OFF_LENGTH(%esi)

        movw   COUNTDOWN(%ebx),%ax
        movw    %ax,COUNTDOWN(%esi)

        movw   STATUS(%ebx),%ax
        movw   %ax,STATUS(%esi)

        cmpl   $0,%eax 
        je     print_off # printing correct character based on status

        movb   ON_CHAR(%ebx),%cl

        jmp    syncpoke

print_off:

        movb   OFF_CHAR(%ebx),%cl

syncpoke:
        
        xorl   %eax,%eax
        movw   LOCATION(%ebx),%ax # outputting sync'd character
        addw   %ax,%ax
        call   mp1_poke
        movl   $0,%eax
        jmp    sync_end

sync_error:
        movl    $-1,%eax

sync_end:
        popl    %edi # restoring callee save
        popl    %esi
        popl    %ebx

        leave
        ret

jumptable:
        .long mp1_ioctl_add, mp1_ioctl_remove, mp1_ioctl_find, mp1_ioctl_sync

.end
