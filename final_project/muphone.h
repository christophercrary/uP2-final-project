/************************************************************************************
 *  File name: muphone.h
 *  Modified: 22 November 2017
 *  Author:  Christopher Crary
 *  Purpose: To declare useful and necessary functions/miscellaneous things
 *           for the MuPhone smartphone device.
************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////

#include "msp.h"    // used for necessary data types
#include <stdlib.h>
#include <stdio.h>      // used for specific wrapper functions, such as sprintf
#include "G8RTOS.h"     // RTOS used to initialize the MSP432 and control the MuPhone
#include "BSP.h"        // used to control MSP432 device (QUESTION: is this needed??)

/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef MUPHONE_H_
#define MUPHONE_H_

//defines for home screen apps
#define MU_MESSAGE_APP_X_START        50
#define MU_MESSAGE_APP_X_END          100
#define MU_MESSAGE_APP_Y_START        50
#define MU_MESSAGE_APP_Y_END          100
#define MU_MESSAGE_APP_SIZE           (MU_MESSAGE_APP_X_END-MU_MESSAGE_APP_X_START)

#define PONG_APP_X_START               200
#define PONG_APP_X_END                 250
#define PONG_APP_Y_START                50
#define PONG_APP_Y_END                 100
#define PONG_APP_SIZE               (PONG_APP_X_END-PONG_APP_X_START)

/* boundaries for LCD screen */
#define LCD_SCREEN_X_MIN 0
#define LCD_SCREEN_X_MAX 320
#define LCD_SCREEN_Y_MIN 0
#define LCD_SCREEN_Y_MAX 240

/* LCD text */
#define LCD_TEXT_WIDTH      8       // in terms of pixels
#define LCD_TEXT_HEIGHT     16
#define LCD_TEXT_COLOR      LCD_BLACK

/* defined color palette */
#define MUPHONE_HOME_SCREEN_BACKGROUND_COLOR                    LCD_BLACK
#define MUPHONE_HEADER_BAR_COLOR                                LCD_BLACK
#define MUPHONE_HEADER_BAR_DIVIDER_COLOR                        LCD_BLACK
#define MUPHONE_HEADER_BAR_NOTIFICATION_TEXT_COLOR              LCD_WHITE
#define MUPHONE_HEADER_BAR_IMPORTANT_NOTIFICATION_TEXT_COLOR    LCD_RED

/* boundaries of phone header bar */
#define MUPHONE_HEADER_BAR_WIDTH            (LCD_SCREEN_X_MAX - LCD_SCREEN_X_MIN)
#define MUPHONE_HEADER_BAR_HEIGHT           20
#define MUPHONE_HEADER_BAR_X_MIN            LCD_SCREEN_X_MIN
#define MUPHONE_HEADER_BAR_X_MAX            (LCD_SCREEN_X_MIN + MUPHONE_HEADER_BAR_WIDTH)
#define MUPHONE_HEADER_BAR_Y_MIN            LCD_SCREEN_Y_MIN
#define MUPHONE_HEADER_BAR_Y_MAX            (MUPHONE_HEADER_BAR_Y_MIN + MUPHONE_HEADER_BAR_HEIGHT)

#define MUPHONE_HEADER_BAR_DIVIDER_WIDTH        (MUPHONE_HEADER_BAR_X_MAX - MUPHONE_HEADER_BAR_X_MIN)
#define MUPHONE_HEADER_BAR_DIVIDER_HEIGHT       1
#define MUPHONE_HEADER_BAR_DIVIDER_X_MIN        MUPHONE_HEADER_BAR_X_MIN
#define MUPHONE_HEADER_BAR_DIVIDER_X_MAX        (MUPHONE_HEADER_BAR_DIVIDER_X_MIN + MUPHONE_HEADER_BAR_DIVIDER_WIDTH)
#define MUPHONE_HEADER_BAR_DIVIDER_Y_MIN        MUPHONE_HEADER_BAR_Y_MAX
#define MUPHONE_HEADER_BAR_DIVIDER_Y_MAX        (MUPHONE_HEADER_BAR_DIVIDER_Y_MIN + MUPHONE_HEADER_BAR_DIVIDER_HEIGHT)

#define MUPHONE_HEADER_BAR_CLOCK_PANEL_OFFSET      2       // spacing used for edges of button
#define MUPHONE_HEADER_BAR_CLOCK_PANEL_WIDTH       ((LCD_TEXT_WIDTH * 18) + (MUPHONE_HEADER_BAR_CLOCK_PANEL_OFFSET * 2))
#define MUPHONE_HEADER_BAR_CLOCK_PANEL_X_MIN       (MUPHONE_HEADER_BAR_X_MAX - MUPHONE_HEADER_BAR_CLOCK_PANEL_WIDTH)
#define MUPHONE_HEADER_BAR_CLOCK_PANEL_X_MAX       MUPHONE_HEADER_BAR_X_MAX
#define MUPHONE_HEADER_BAR_CLOCK_PANEL_Y_MIN       MUPHONE_HEADER_BAR_Y_MIN
#define MUPHONE_HEADER_BAR_CLOCK_PANEL_Y_MAX       (MUPHONE_HEADER_BAR_MESSAGE_NOTIFICATION_PANEL_Y_MIN + LCD_TEXT_HEIGHT + MUPHONE_HEADER_BAR_MESSAGE_NOTIFICATION_PANEL_OFFSET)

#define MUPHONE_HEADER_BAR_CLOCK_TEXT_X_OFFSET       2
#define MUPHONE_HEADER_BAR_CLOCK_TEXT_Y_OFFSET       2
#define MUPHONE_HEADER_BAR_CLOCK_TEXT_X_START        (MUPHONE_HEADER_BAR_CLOCK_PANEL_X_MIN + MUPHONE_HEADER_BAR_CLOCK_TEXT_X_OFFSET)
#define MUPHONE_HEADER_BAR_CLOCK_TEXT_Y_START        (MUPHONE_HEADER_BAR_CLOCK_PANEL_Y_MIN + MUPHONE_HEADER_BAR_CLOCK_TEXT_Y_OFFSET)

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
* Name: muphone
* Purpose: User function to initialize the MuPhone, and turn on the device.
* Input(s): N/A
* Output: N/A
************************************************************************************/
void muphone(void);

//////////////////////////END OF PUBLIC FUNCTION PROTOTYPES//////////////////////////

////////////////////////////////////THREADS//////////////////////////////////////////

/****************************** APERIODIC THREADS **********************************/

/************************** END OF APERIODIC THREADS *******************************/


/******************************* COMMON THREADS ************************************/

/************************************************************************************
* Name: thread_muphone_idle
* Purpose: Permanently active thread to avoid system termination or deadlock
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_muphone_idle(void);

/************************************************************************************
* Name: thread_muphone_lock_screen
* Purpose: Thread to handle the MuPhone's lock screen
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_muphone_lock_screen(void);

/************************************************************************************
* Name: thread_muphone_home_screen
* Purpose: Thread to handle the MuPhone's home screen
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_muphone_home_screen(void);

/**************************** END OF COMMON THREADS ********************************/


/******************************** HOST THREADS *************************************/

/***************************** END OF HOST THREADS *********************************/


/******************************* CLIENT THREADS ************************************/

/**************************** END OF CLIENT THREADS ********************************/


/////////////////////////////////END OF THREADS//////////////////////////////////////

#endif      // end of header guard

////////////////////////////////END OF muphone.h////////////////////////////////////
