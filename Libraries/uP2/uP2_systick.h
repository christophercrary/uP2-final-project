/************************************************************************************
 *  File name: uP2_systick.h
 *  Modified: 03 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To declare useful functions for the SysTick system in the MSP432.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "msp.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef UP2_SYSTICK_H
#define UP2_SYSTICK_H

// number of clock cycles running at 48MHz to achieve 1ms delay
#define NUM_CLOCK_CYCLES_48MHZ_1MS            50000

////////////////////////////////END OF DEFINES///////////////////////////////////////


//////////////////////////////////INITIALIZATIONS////////////////////////////////////
// systick error enumeration
typedef enum { systick_err = -1, no_systick_err = 0 }systick_response_code_t;
/////////////////////////////END OF INITIALIZATIONS//////////////////////////////////

////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////


///////////////////////////////FUNCTION PROTOTYPES///////////////////////////////////

/************************************************************************************
* Name: systick_int_enable()
* Purpose: To enable the SysTick TICK interrupt in the SysTick system in the  MSP432.
*          Note: Global interrupts are not yet enabled (I bit is not cleared) in this
*          function. User must call __enable_irq() or some other function.
* Input(s): N/A
* Output: N/A
************************************************************************************/
systick_response_code_t systick_int_enable(uint32_t priority);

/************************************************************************************
 * Name: systick_init()
 * Purpose: To initialize/enable the SysTick system in the MSP432. Programmer decides
 *          period of SysTick underflow, in terms of clock cycles. Note: Programmer
 *          must be aware of system_clock_frequency and is responsible for calculating
 *          correct number of clock cycles.
 * Input(s): uint32_t clock_cycles
 * Output: systick_response_code_t response_code
 ************************************************************************************/
systick_response_code_t systick_init(uint32_t clock_cycles);


////////////////////////////END OF FUNCTION PROTOTYPES///////////////////////////////

#endif		// end of header guard

////////////////////////////////END OF uP2_systick.h////////////////////////////////////
