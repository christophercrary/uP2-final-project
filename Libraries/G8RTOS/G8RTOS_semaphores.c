/************************************************************************************
 *  File name: G8RTOS_semaphores.c
 *  Modified: 12 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To define useful functions for semaphores in the G8RTOS.
 ************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////

#include "G8RTOS_semaphores.h"
#include "msp.h"
#include "G8RTOS_critical_section.h"        // used to start/stop atomic sections
#include "G8RTOS_scheduler.h"           // used to reference G8RTOS_CONTEXT_SWITCH()
#include "G8RTOS_structures.h"          // used to reference running_thread_ptr

/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////

// trigger PENDSV_handler to switch thread contexts
#define G8RTOS_CONTEXT_SWITCH()     SCB->ICSR |= 0x10000000;    // set PendSV bit

////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////

extern tcb_t *running_thread_ptr;       // declared in G8RTOS_structures.h

////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////
//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////


////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////

/************************************************************************************
 * Name: G8RTOS_semaphore_init
 * Purpose: To initialize a semaphore to a given value
 * Input(s): semaphore_t *s, int32_t value
 * Output: N/A
 ************************************************************************************/
void G8RTOS_semaphore_init(semaphore_t *s, int32_t value)
{
    int32_t primask_status;        // used to store PRIMASK (mainly for I bit)
    primask_status = G8RTOS_START_CRITICAL_SECTION();       // start atomic operations

    (*s) = value;     // initialize given semaphore to given value
    
    G8RTOS_END_CRITICAL_SECTION(primask_status);        // end atomic operations

    return;
}

/************************************************************************************
 * Name: G8RTOS_semaphore_wait
 * Purpose: Utilize blocking semaphores for shared resources.
 * Input(s): semaphore_t *s
 * Output: N/A
 ************************************************************************************/
void G8RTOS_semaphore_wait(semaphore_t *s)
{
    int32_t primask_status;        // used to store PRIMASK (mainly for I bit)
    primask_status = G8RTOS_START_CRITICAL_SECTION();       // start atomic operations
    
    (*s) = (*s) - 1;        // signal that thread is attempting to utilize resource
    
    if ((*s) < 0)           // check if semaphore should block sought resource
    {
        // semaphore should be blocked, update currently running thread to track this
        running_thread_ptr->blocked = s;
        
        // allow for context switching and other interrupts to occur
        G8RTOS_END_CRITICAL_SECTION(primask_status);
        
        // suspend thread to allow for next thread to run
        // this method does not give the next thread a full 1ms time-slot
        // implementation of G8RTOS_thread_suspend would fix this issue
        G8RTOS_CONTEXT_SWITCH();
    }
    
    // only located here if resource wasn't blocked
    
    G8RTOS_END_CRITICAL_SECTION(primask_status);        // end atomic operations
    
    return;
}

/************************************************************************************
 * Name: G8RTOS_semaphore_signal
 * Purpose: Signals unblocking of a resource using a blocking semaphore.
 *          This function finds the first blocked thread on semaphore s, and unblocks
 *          this thread.
 * Input(s): semaphore_t *s
 * Output: N/A
 ************************************************************************************/
void G8RTOS_semaphore_signal(semaphore_t *s)
{
    tcb_t *temp;        // temporary TCB pointer used to traverse list of TCBs
                        // as to determine which TCB to signal
    
    int32_t primask_status;        // used to store PRIMASK (mainly for I bit)
    primask_status = G8RTOS_START_CRITICAL_SECTION();       // start atomic operations
    
    (*s) = (*s) + 1;        // increment semaphore signaling its availability
    
    // determine if threads are currently blocked on passed-in semaphore s
    if ((*s) <= 0)
    {
        temp = running_thread_ptr->next;        // always update to next TCB
        
        // traverse TCB list until first TCB blocked on semaphore s is found
        while (temp->blocked != s)
        {
            temp = temp->next;      // go to next TCB in TCB list
        }
        
        // unblock first found TCB that is blocked on semaphore s
        temp->blocked = 0;      // wake up first found thread
    }
    
    G8RTOS_END_CRITICAL_SECTION(primask_status);        // end atomic operations
    
    return;
}

/////////////////////////////END OF PUBLIC FUNCTIONS/////////////////////////////////


/////////////////////////////////PRIVATE FUNCTIONS///////////////////////////////////
//////////////////////////////END OF PRIVATE FUNCTIONS///////////////////////////////


////////////////////////////INTERRUPT SERVICE ROUTINES///////////////////////////////
////////////////////////END OF INTERRUPT SERVICE ROUTINES////////////////////////////

/////////////////////////////END OF G8RTOS_semaphores.c//////////////////////////////
