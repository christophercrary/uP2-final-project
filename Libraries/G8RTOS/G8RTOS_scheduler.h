/************************************************************************************
 *  File name: G8RTOS_scheduler.h
 *  Modified: 10 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To declare useful functions for scheduling threads in the G8RTOS.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "G8RTOS_structures.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef G8RTOS_SCHEDULER_H_
#define G8RTOS_SCHEDULER_H_

#define NULL    0       // unnecessary to include stdint.h
#define OSINT_PRIORITY 7    // SysTick interrupt priority
#define MAX_APERIODIC_THREAD_PRIORITY   6       // max user aperiodic thread priority
#define MAX_THREAD_PRIORITY     255     // max background thread priority

// OS memory limits
#define MAX_THREADS 30       // max number of task threads
#define STACKSIZE 256      // max stack size of each task thread
#define MAX_PETS    6       // max number of periodic event threads

// define useful intervals of time in terms of milliseconds
#define ZERO_SECONDS                0
#define TENTH_SECOND_MS             100
#define TWO_TENTHS_SECOND_MS        200
#define QUARTER_SECOND_MS           250
#define FOUR_TENTHS_SECOND_MS       400
#define HALF_SECOND_MS              500
#define ONE_SECOND_MS               1000
#define TWO_SECONDS_MS              2000
#define FIVE_SECONDS_MS             5000
#define TEN_SECONDS_MS              10000
////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////
//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////


////////////////////////////PUBLIC FUNCTION PROTOTYPES///////////////////////////////

/************************************************************************************
 * Name: G8RTOS_add_aperiodic_thread
 * Purpose: Function to potentially add an aperiodic thread to the scheduler.
 *          The aperiodic thread must configure a valid IRQ number (0 - 40)
 *          in the MSP432 processor.
 *          The aperiodic thread must have a valid user IRQ priority
 *          (0 <= priority < 7) (priority = 7 is reserved for the SysTick ISR)
 * Input(s): void (*aperiodic_thread_function)(void),
             uint8_t priority, IRQn_Type IRQn
 * Output: G8RTOS_response_code_t response_code
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_add_aperiodic_thread(void (*aperiodic_thread_function)(void),
                                                   IRQn_Type IRQn, uint8_t priority);

/************************************************************************************
 * Name: G8RTOS_disable_aperiodic_thread
 * Purpose: Function to disable a specific aperiodic thread's IRQ
 * Input(s): IRQn_Type IRQn
 * Output: G8RTOS_response_code_t response_code
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_disable_aperiodic_thread(IRQn_Type IRQn);

/************************************************************************************
 * Name: G8RTOS_disable_aperiodic_thread
 * Purpose: Function to enable a specific aperiodic thread's IRQ.
 *          Note: the aperiodic thread MUST have already been added by making a call
 *          to G8RTOS_add_aperiodic_event.
 * Input(s): IRQn_Type IRQn
 * Output: G8RTOS_response_code_t response_code
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_enable_aperiodic_thread(IRQn_Type IRQn);

/************************************************************************************
 * Name: G8RTOS_add_pet
 * Purpose: Potentially add a periodic event thread to the array of periodic event
 *          threads PET[].
 * Input(s): void (*handler)(void), uint32_t period, uint32_t time_offset
 * Output: G8RTOS_response_code_t response_code
 ***********************************************************************************/
G8RTOS_response_code_t G8RTOS_add_pet(void (*handler)(void), uint32_t period,
                                      uint32_t time_offset, char *pet_name);

/************************************************************************************
 * Name: G8RTOS_add_thread
 * Purpose: Function to potentially add a thread to the scheduler. If a TCB can be
 *          added, all of the TCB's data members are set up and initialized.
 *          A void pointer to a task (a function) is passed into G8RTOS_add_thread.
 *          This pointer will be assigned to the PC of the thread's TCB.
 * Input(s): void (*thread_function)(void)
 * Output: G8RTOS_response_code_t response_code
 ***********************************************************************************/
G8RTOS_response_code_t G8RTOS_add_thread(void (*thread_function)(void),
                                         uint8_t priority, char *thread_name);

/************************************************************************************
* Name: G8RTOS_get_tid
* Purpose: Get thread ID of currently running thread (running_thread_ptr)
* Input(s): N/A
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
tid_t G8RTOS_get_tid(void);

pid_t G8RTOS_get_pid();

/************************************************************************************
* Name: G8RTOS_init
* Purpose: Initializes necessary variables and hardware for G8RTOS.
* Input(s): N/A
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_init(void);

/************************************************************************************
* Name: G8RTOS_kill_all_other_threads
* Purpose: Attempts to kill all other threads besides currently running thread.
* Input(s): N/A
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_kill_all_other_threads();

/************************************************************************************
* Name: G8RTOS_kill_pet
* Purpose: Attempts to kill specified PET
* Input(s): pid_t pet_id
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_kill_pet(pid_t pet_id);

/************************************************************************************
* Name: G8RTOS_kill_current_thread
* Purpose: Attempts to kill currently running thread.
* Input(s): N/A
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_kill_current_thread(void);

/************************************************************************************
* Name: G8RTOS_kill_thread
* Purpose: Attempts to kill specified thread
* Input(s): tid_t thread_id
* Output: G8RTOS_response_code_t response_code
************************************************************************************/
G8RTOS_response_code_t G8RTOS_kill_thread(tid_t thread_id);

/************************************************************************************
 * Name: G8RTOS_launch
 * Purpose: Launch the G8RTOS system. The SysTick system is configured, and the
 *          context is set to the first thread. If there are no active threads,
 *          function returns missing_thread_err (-4).
 * Input(s): N/A
 * Output: G8RTOS_response_code_t response_code
 ***********************************************************************************/
G8RTOS_response_code_t G8RTOS_launch(void);

/************************************************************************************
 * Name: G8RTOS_thread_sleep
 * Purpose: To sleep a currently running thread, and allow the G8RTOS to remain
 *          for a finite amount of time. A thread in the sleep state will NOT be run.
 *          The thread will become active again in the G8RTOS_scheduler function,
 *          after the system_time matches the sleep_finish_time, which is located
 *          in the specific TCB's structure.
 * Input(s): semaphore_t *s
 * Output: N/A
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_thread_sleep(uint32_t sleep_duration);

/************************************************************************************
 * Name: G8RTOS_thread_suspend
 * Purpose: To suspend a currently running thread, and allow the G8RTOS to run the
 *          next active thread. This function is called whenever a currently running
 *          thread's needed resource is blocked.
 * Input(s): semaphore_t *s
 * Output: N/A
 ************************************************************************************/
G8RTOS_response_code_t G8RTOS_thread_suspend(void);

//////////////////////////END OF PUBLIC FUNCTION PROTOTYPES//////////////////////////

#endif		// end of header guard

/////////////////////////////END OF G8RTOS_scheduler.h///////////////////////////////
