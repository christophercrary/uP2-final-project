/************************************************************************************
 *  File name: G8RTOS_semaphores.h
 *  Modified: 12 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To declare useful semaphore functions for the G8RTOS.
 ************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////
#include <stdint.h>
/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef G8RTOS_SEMAPHORES_H_
#define G8RTOS_SEMAPHORES_H_
////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////

typedef int32_t semaphore_t;        // alternate name for int32_t

//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////


////////////////////////////PUBLIC FUNCTION PROTOTYPES///////////////////////////////

/************************************************************************************
 * Name: G8RTOS_semaphore_init
 * Purpose: To initialize a semaphore to a given value
 * Input(s): semaphore_t *s, int32_t value
 * Output: N/A
 ************************************************************************************/
void G8RTOS_semaphore_init(semaphore_t *s, int32_t value);

/************************************************************************************
 * Name: G8RTOS_semaphore_wait
 * Purpose: Spinlock waits until semaphore becomes available. When semaphore is
 *          available, decrements semaphore value
 * Input(s): semaphore_t *s
 * Output: N/A
 ************************************************************************************/
void G8RTOS_semaphore_wait(semaphore_t *s);

/************************************************************************************
 * Name: G8RTOS_semaphore_signal
 * Purpose: Signals completion of semaphore usage by incrementing semaphore value.
 * Input(s): semaphore_t *s
 * Output: N/A
 ************************************************************************************/
void G8RTOS_semaphore_signal(semaphore_t *s);

//////////////////////////END OF PUBLIC FUNCTION PROTOTYPES//////////////////////////

#endif		// end of header guard

////////////////////////////////END OF FILENAME.h////////////////////////////////////
