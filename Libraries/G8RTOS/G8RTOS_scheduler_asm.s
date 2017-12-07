;************************************************************************************
;  File name: G8RTOS_scheduler_asm.s
;  Modified: 11 September 2017
;  Author:  Christopher Crary
;  Purpose: To define useful assembly functions for the G8RTOS scheduler.
;************************************************************************************


;*******************************FUNCTION DEFINITIONS*********************************

    .def G8RTOS_START, PendSV_Handler

;****************************END OF FUNCTION DEFINITIONS*****************************


;************************************DEPENDENCIES************************************

	.ref running_thread_ptr, G8RTOS_scheduler

;********************************END OF DEPENDENCIES*********************************


;********************************ASSEMBLER DIRECTIVES********************************


    .THUMB		; Set to thumb mode
    .ALIGN 2	; Align by 2 bytes (thumb mode uses alignment by 2 or 4)
    .TEXT		; Program memory

    ; Need to have the running_thread_ptr address defined in file
    ; (label needs to be close enough to asm code to be reached with PC relative
    ; addressing)
RUNNINGPTR: .field running_thread_ptr, 32       ; define a 32-bit bit-field

;****************************END OF ASSEMBLER DIRECTIVES*****************************

;*******************************FUNCTION DEFINITIONS*********************************

;************************************************************************************
; Name: G8RTOS_START
; Purpose: To set the first thread in tcb_list to be the currently running thread.
;          It is the responsibility of the programmer to set RUNNINGPTR to point
;          to the first TCB in the tcb_list.
; Input(s): RUNNINGPTR
; Output: N/A
;************************************************************************************
G8RTOS_START:

    .asmfunc

    LDR R0, RUNNINGPTR     ; (address of first running thread) -> R0
    LDR R1, [R0]            ; (address of first thread stack pointer) -> R1
    LDR SP, [R1]            ; load processor stack pointer (SP = RunningThreadPtr->sp)
    POP {R4-R11}            ; restore registers R4-R11
    POP {R0-R3}             ; restore registers R0-R3
    POP {R12}
    ADD SP, SP, #4          ; discard LR from initial stack
    POP {LR}                ; store task's function address location into LR
    ADD SP, SP, #4          ; discard initial PSR
    CPSIE I                 ; enable interrupts
    BX LR                   ; start first thread

    .endasmfunc


;************************************************************************************
; Name: PendSV_Handler
; Purpose: To perform a context switch between TCBs in the G8RTOS.
; Input(s): RUNNINGPTR
; Output: N/A
;************************************************************************************
PendSV_Handler:

    .asmfunc

    ; registers R0-R3, R12, LR, PC, and PSR automatically pushed in subroutines

    CPSID I                	; start of critical section (prevent interrupts)
    PUSH {R4-R11}          	; protect necessary registers
    LDR R0, RUNNINGPTR     	; (address of current running thread) -> R0
    LDR R1, [R0]           	; (address of current thread stack pointer) -> R1
    STR SP, [R1]           	; repoint current thread stack pointer
    PUSH {R0, LR}          	; protect return address (push EVEN amount of registers)
    BL G8RTOS_scheduler    	; running_thread_ptr = running_thread_ptr->next;
    POP {R0, LR}           	; restore return address
    LDR R1, [R0]           	; load address of next thread stack pointer into R1
    LDR SP, [R1]            	; repoint processor's stack pointer to next thread's
    POP {R4-R11}            	; restore necessary registers
    CPSIE I                 	; end of critical section (re-enable interrupts)
    BX LR                   	; restore registers R0-R3, R12, LR, PC, PSR
                            	; and start next thread

    .endasmfunc

;****************************END OF FUNCTION DEFINITIONS*****************************

;******************************MORE ASSEMBLER DIRECTIVES*****************************

    .ALIGN      ; align again (not sure why)
    .END        ; end the ASM file

;**************************END OF MORE ASSEMBLER DIRECTIVES**************************

;*****************************END OF G8RTOS_scheduler_asm.s**************************
