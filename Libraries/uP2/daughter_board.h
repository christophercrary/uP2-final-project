/************************************************************************************
 *  File name: daughter_board.h
 *  Modified: 22 October 2017
 *  Author:  Christopher Crary
 *  Purpose: To declare useful functions for the uP2 daughter board.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "msp.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef DAUGHTER_BOARD_H
#define DAUGHTER_BOARD_H
////////////////////////////////END OF DEFINES///////////////////////////////////////


//////////////////////////////////INITIALIZATIONS////////////////////////////////////
// error response code enumeration for the daughter board
typedef enum { daughter_board_err = -1, no_daughter_board_err = 0 }daughter_board_response_t;
/////////////////////////////END OF INITIALIZATIONS//////////////////////////////////

////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////


///////////////////////////////FUNCTION PROTOTYPES///////////////////////////////////

/************************************************************************************
* Name: button_up_int_enable
* Purpose: To enable an external interrupt for the top push-button on the daughter
*          board. This corresponds with Port 4, pin 4. The user can decide what
*          priority is assigned to this interrupt.
*          Note: This subroutine does not enable the necessary port IRQ.
*                It is the user's responsibility to add the necessary aperiodic
*                thread before using the specified button.
* Input(s): uint32_t priority
* Output: daughter_board_response_t response_code
************************************************************************************/
daughter_board_response_t button_up_int_enable(uint32_t priority);

/************************************************************************************
* Name: button_down_int_enable
* Purpose: To enable an external interrupt for the bottom push-button on the daughter
*          board. This corresponds with Port 5, pin 4. The user can decide what
*          priority is assigned to this interrupt.
*          Note: This subroutine does not enable the necessary port IRQ.
*                It is the user's responsibility to add the necessary aperiodic
*                thread before using the specified button.
* Input(s): uint32_t priority
* Output: daughter_board_response_t response_code
************************************************************************************/
inline daughter_board_response_t button_down_int_enable(uint32_t priority);

/************************************************************************************
* Name: button_left_int_enable
* Purpose: To enable an external interrupt for the left push-button on the daughter
*          board. This corresponds with Port 5, pin 5. The user can decide what
*          priority is assigned to this interrupt.
*          Note: This subroutine does not enable the necessary port IRQ.
*                It is the user's responsibility to add the necessary aperiodic
*                thread before using the specified button.
* Input(s): uint32_t priority
* Output: daughter_board_response_t response_code
************************************************************************************/
inline daughter_board_response_t button_left_int_enable(uint32_t priority);

/************************************************************************************
* Name: button_right_int_enable
* Purpose: To enable an external interrupt for the right push-button on the daughter
*          board. This corresponds with Port 4, pin 5. The user can decide what
*          priority is assigned to this interrupt.
*          Note: This subroutine does not enable the necessary port IRQ.
*                It is the user's responsibility to add the necessary aperiodic
*                thread before using the specified button.
* Input(s): uint32_t priority
* Output: daughter_board_response_t response_code
************************************************************************************/
inline daughter_board_response_t button_right_int_enable(uint32_t priority);

/************************************************************************************
* Name: buttons_all_int_enable
* Purpose: To enable external interrupts for all push-buttons on the daughter
*          board. The user can decide one priority to be assigned to all button
*          interrupts.
*          Note: This subroutine does not enable the necessary port IRQs.
*                It is the user's responsibility to add the necessary aperiodic
*                threads before using the specified buttons.
* Input(s): uint32_t priority
* Output: daughter_board_response_t response_code
************************************************************************************/
inline daughter_board_response_t button_all_int_enable(uint32_t priority);

////////////////////////////END OF FUNCTION PROTOTYPES///////////////////////////////

#endif		// end of header guard

////////////////////////////////END OF daughter_board.h////////////////////////////////////
