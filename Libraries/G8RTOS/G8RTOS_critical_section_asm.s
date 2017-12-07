;************************************************************************************
;  File name: G8RTOS_critical_section_asm.s
;  Modified: 12 September 2017
;  Author:  Christopher Crary
;  Purpose: To define useful functions to start/stop sections of atomic operations.
;           Atomic operations are operations that are guaranteed to complete without
;           being interrupted.
;************************************************************************************


;***********************************FUNCTION DEFINES*********************************

    ; ASM function definition(s)
    .def G8RTOS_START_CRITICAL_SECTION, G8RTOS_END_CRITICAL_SECTION

;********************************END OF FUNCTION DEFINES*****************************


;********************************ASSEMBLER DIRECTIVES********************************

    .THUMB      ; select THUMB instruction set
    .ALIGN 2    ; instructions are 16-bits wide (2 bytes), align them in memory
    .TEXT       ; tell the processor that you are writing in program memory

;****************************END OF ASSEMBLER DIRECTIVES*****************************


;*******************************FUNCTION DEFINITIONS*********************************

;************************************************************************************
; Name: G8RTOS_START_CRITICAL_SECTION
; Purpose: Saves PRIMASK status before starting a critical section.
; Input(s): N/A
; Output: PRIMASK (R0)
;************************************************************************************
G8RTOS_START_CRITICAL_SECTION:

    .asmfunc    ; anything before .endasmfunc will be part of the ASM function

    MRS R0, PRIMASK		; Save PRIMASK to R0 (Return Register)
    CPSID I				; Disable Interrupts
    BX LR				; Return

    .endasmfunc	; end the ASM function definition

;************************************************************************************
; Name: G8RTOS_END_CRITICAL_SECTION
; Purpose: Restores previous state of PRIMASK before critical section began.
; Input(s): PRIMASK (R0)
; Output: N/A
;************************************************************************************
G8RTOS_END_CRITICAL_SECTION:

    .asmfunc    ; anything before .endasmfunc will be part of the ASM function

    MSR PRIMASK, R0		; Save R0 (Param) to PRIMASK
    BX LR				; Return

    .endasmfunc	; end the ASM function definition


;****************************END OF FUNCTION DEFINITIONS*****************************


;******************************MORE ASSEMBLER DIRECTIVES*****************************

    .ALIGN      ; align again (not sure why)
    .END        ; end the ASM file

;**************************END OF MORE ASSEMBLER DIRECTIVES**************************

;***********************END OF G8RTOS_critical_section_asm.s*************************
