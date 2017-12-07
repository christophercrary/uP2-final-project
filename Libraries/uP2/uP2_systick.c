/************************************************************************************
 *  File name: uP2_systick.c
 *  Modified: 03 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To define useful functions for the SysTick system in the MSP432.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "msp.h"
#include "uP2_systick.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
#define OSINT_PRIORITY      7
////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////
//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////

/************************************************************************************
* Name: systick_int_enable()
* Purpose: To enable the SysTick TICK interrupt in the SysTick system in the  MSP432.
*          Note: Global interrupts are not yet enabled (I bit is not cleared) in this
*          function. User must call __enable_irq() or some other function.
* Input(s): N/A
* Output: N/A
************************************************************************************/
systick_response_code_t systick_int_enable(uint32_t priority)
{
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;   // enable the TICK interrupt
  NVIC_SetPriority(SysTick_IRQn, priority);   // set interrupt priority

  return no_systick_err;
}

/************************************************************************************
 * Name: systick_init()
 * Purpose: To initialize/enable the SysTick system in the MSP432. This function
 *          also enables the SysTick TICK interrupt. The programmer decides the
 *          period of SysTick underflow, in terms of clock cycles. Note: Programmer
 *          must be aware of system_clock_frequency and is responsible for calculating
 *          correct number of clock cycles.
 * Input(s): uint32_t clock_cycles
 * Output: systick_response_code_t response_code
 ************************************************************************************/
systick_response_code_t systick_init(uint32_t clock_cycles)
{
    if (clock_cycles <= 0)
    {
        return systick_err;
    }
    else
    {
        SysTick->CTRL = 0;      // disable the SysTick system
        SysTick->VAL = 0;       // reset the SysTick counter (CVR)
        SysTick->LOAD = clock_cycles - 1;       // load the RVR register
        SysTick->CTRL |= (SysTick_CTRL_CLKSOURCE_Msk |
                          SysTick_CTRL_TICKINT_Msk   |
                          SysTick_CTRL_ENABLE_Msk);   // system_clock, tick interrupt, enable
        NVIC_SetPriority(SysTick_IRQn, OSINT_PRIORITY);   // set interrupt priority
    }
    
    return no_systick_err;
}

/////////////////////////////END OF PUBLIC FUNCTIONS/////////////////////////////////

/////////////////////////////////PRIVATE FUNCTIONS///////////////////////////////////
//////////////////////////////END OF PRIVATE FUNCTIONS///////////////////////////////


////////////////////////////INTERRUPT SERVICE ROUTINES///////////////////////////////
////////////////////////END OF INTERRUPT SERVICE ROUTINES////////////////////////////


////////////////////////////////END OF uP2_systick.c////////////////////////////////////
