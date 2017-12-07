/************************************************************************************
*  File name: daughter_board.h
*  Modified: 22 October 2017
*  Author:  Christopher Crary
*  Purpose: To define useful functions for the uP2 daughter board.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "daughter_board.h"
#include "msp.h"
#include "gpio.h"       // used for configuring the daughter board buttons
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
////////////////////////////////END OF DEFINES///////////////////////////////////////

//////////////////////////////////INITIALIZATIONS////////////////////////////////////
/////////////////////////////END OF INITIALIZATIONS//////////////////////////////////

///////////////////////////////FUNCTION DEFINITIONS//////////////////////////////////

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
daughter_board_response_t button_up_int_enable(uint32_t priority)
{
    // configure specified daughter board button to be an input (see daughter board schematic)
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN4);

    // configure falling edge interrupts for specified daughter board button
    GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN4, GPIO_HIGH_TO_LOW_TRANSITION);

    // enable interrupts for daughter board buttons (does not enable PORT4 IRQ!)
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN4);

    return no_daughter_board_err;
}

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
inline daughter_board_response_t button_down_int_enable(uint32_t priority)
{
    // configure specified daughter board button to be an input (see daughter board schematic)
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN4);

    // configure falling edge interrupts for specified daughter board button
    GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN4, GPIO_HIGH_TO_LOW_TRANSITION);

    // enable interrupts for daughter board buttons (does not enable PORT5 IRQ!)
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN4);

    return no_daughter_board_err;
}

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
inline daughter_board_response_t button_left_int_enable(uint32_t priority)
{
    // configure specified daughter board button to be an input (see daughter board schematic)
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN5);

    // configure falling edge interrupts for specified daughter board button
    GPIO_interruptEdgeSelect(GPIO_PORT_P5, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);

    // enable interrupts for daughter board buttons (does not enable PORT5 IRQ!)
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN5);

    return no_daughter_board_err;
}

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
inline daughter_board_response_t button_right_int_enable(uint32_t priority)
{
    // configure specified daughter board button to be an input (see daughter board schematic)
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN5);

    // configure falling edge interrupts for specified daughter board button
    GPIO_interruptEdgeSelect(GPIO_PORT_P4, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);

    // enable interrupts for daughter board buttons (does not enable PORT4 IRQ!)
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN5);

    return no_daughter_board_err;
}

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
inline daughter_board_response_t button_all_int_enable(uint32_t priority)
{
    // enable top button (w/interrupt) (port 4, pin 4)
    button_up_int_enable(priority);

    // enable bottom button (w/interrupt) (port 5, pin 4)
    button_down_int_enable(priority);

    // enable left button (w/interrupt) (port 5, pin 5)
    button_left_int_enable(priority);

    // enable right button (w/interrupt) (port 4, pin 5)
    button_right_int_enable(priority);

    return no_daughter_board_err;
}
////////////////////////////END OF FUNCTION DEFINITIONS//////////////////////////////

//////////////////////////END OF daughter_board.c////////////////////////////////////
