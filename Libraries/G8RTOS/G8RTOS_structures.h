/************************************************************************************
 *  File name: G8RTOS_structures.h
 *  Modified: 10 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To define useful structures used in the G8RTOS.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "msp.h"
#include "G8RTOS_semaphores.h"  // used for referencing semaphore_t
#include <stdbool.h>
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef G8RTOS_STRUCTURES_H_
#define G8RTOS_STRUCTURES_H_

#define STACKSIZE          256
#define FIFOSIZE            16      // as defined in Lab 3
#define MAX_NAME_LENGTH     16      // as defined in Lab 4

////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////
// boolean enum definition
//typedef enum {
 //   FALSE = 0,
  //  TRUE = 1
//}bool_t;

// thread ID definition
typedef uint32_t tid_t;

// PET ID definition
typedef uint32_t pid_t;

// G8RTOS error code enumerations
typedef enum {
    invalid_thread_name = -14,  // invalid thread name length
    invalid_pet_name = -13,     // invalid pet name length
    pet_nonexistent = -12,      // cannot find PET
    cannot_kill_pet = -11,      // attempting to kill nonexistent PET
    aperiodic_invalid_priority = -10,       // invalid aperiodic IRQ priority
    aperiodic_invalid_irq_number = -9,      // attempted to configure an invalid IRQ
    thread_invalid_priority = -8,    // attempted to assign invalid thread priority
    thread_nonexistent = -7,     // cannot find thread
    cannot_kill_thread = -6,   // attempting to kill only thread
    max_pet_err = -5,            // more than MAX_PETS active PETs
    missing_thread_err = -4,     // no active thread
    max_thread_err = -3,         // more than MAX_THREADS active threads
    mem_space_err = -2,          // not enough free memory space
    G8RTOS_err = -1,             // general error
    no_G8RTOS_err = 0            // no error
}G8RTOS_response_code_t;

// Thread Control Block structure (Lab 4)
typedef struct tcb {
    int32_t *sp;    // thread stack pointer
    struct tcb *next;    // pointer to next thread in tcb list
    struct tcb *prev;    // pointer to previous thread in tcb list
    semaphore_t *blocked;    // pointer to blocking semaphore
    uint32_t sleep_finish_time;     // variable to determine if tcb should be asleep
    bool isAsleep;      // boolean to determine if tcb is asleep
    uint8_t priority;       // 0 is the highest, 255 is the lowest
    bool isAlive;     // determines whether thread is dead/alive
    tid_t thread_id;    // unique thread ID assigned to thread
    char thread_name[MAX_NAME_LENGTH];      // unique thread name
}tcb_t;

// Periodic Event Thread structure (Lab 3)
typedef struct pet {
    struct pet *next;       // pointer to next periodic event thread in pet list
    struct pet *prev;       // pointer to previous pet in pet list
    uint32_t time_offset;      // used for pets that share common multiples of time
    uint32_t execution_time;        // used to know when to start pet
    uint32_t period;        // used to know how frequently to run pet
    void (*handler)(void);      // void function pointer to void pet handler
    bool isAlive;     // determines whether thread is dead/alive
    pid_t pet_id;    // unique thread ID assigned to thread
    char pet_name[MAX_NAME_LENGTH];      // unique thread name
}pet_t;

tcb_t *running_thread_ptr;    // pointer to currently running thread TCB

//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////

////////////////////////////PUBLIC FUNCTION PROTOTYPES///////////////////////////////
//////////////////////////END OF PUBLIC FUNCTION PROTOTYPES//////////////////////////

#endif		// end of header guard

//////////////////////////////END OF G8RTOS_structures.h/////////////////////////////
