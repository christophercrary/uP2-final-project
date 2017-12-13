/************************************************************************************
 *  File name: muphone.h
 *  Modified: 22 November 2017
 *  Author:  Christopher Crary
 *  Purpose: To declare useful and necessary functions/miscellaneous things
 *           for the MuPhone smartphone device.
************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////

#include "msp.h"    // used for necessary data types
#include "G8RTOS.h"     // RTOS used to initialize the MSP432 and control the MuPhone
#include "BSP.h"        // used to control MSP432 device (QUESTION: is this needed??)
#include "Applications.h"
#include "cc3100_usage.h"
#include <stdio.h>
#include <stdlib.h>

/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef MUPHONE_H_
#define MUPHONE_H_

// special hardware pin numbers
#define PIN_TOUCHPANEL              0
#define PIN_HOME_BUTTON             4

// special sleep times
#define LCD_DEBOUNCE_TIME           50
/* boundaries for LCD screen */
#define LCD_SCREEN_X_MIN 0
#define LCD_SCREEN_X_MAX 319
#define LCD_SCREEN_Y_MIN 0
#define LCD_SCREEN_Y_MAX 239

/* LCD text */
#define LCD_TEXT_WIDTH      8       // in terms of pixels
#define LCD_TEXT_HEIGHT     16
#define LCD_TEXT_COLOR      LCD_BLACK

/* ASCII defines */
#define LF                  10      // line feed
#define CR                  13      // carriage return

// button indices for demo screen
#define BRIT_BUTTON_INDEX           0
#define CHRIS_BUTTON_INDEX          1

// app indices for home_screen_applications section
#define MUMESSAGE_APP_ICON_INDEX    0
#define BALLSY_APP_ICON_INDEX       1
#define PONG_APP_ICON_INDEX         2

// fuck this
#define MAX_NUMBER_OF_CONTACTS      3

/* defined color palette */
#define MUPHONE_HOME_SCREEN_BACKGROUND_COLOR                    LCD_BLACK
#define MUPHONE_APP_ICON_COLOR                                  LCD_LIGHT_GRAY
#define MUPHONE_APP_ICON_BORDER_COLOR                           LCD_LIGHT_GRAY
#define MUPHONE_APP_ICON_TEXT_COLOR                             LCD_WHITE
#define MUPHONE_HEADER_BAR_COLOR                                LCD_BLACK
#define MUPHONE_HEADER_BAR_DIVIDER_COLOR                        LCD_WHITE
#define MUPHONE_HEADER_BAR_NOTIFICATION_TEXT_COLOR              LCD_WHITE
#define MUPHONE_HEADER_BAR_IMPORTANT_NOTIFICATION_TEXT_COLOR    LCD_WHITE


#define MUPHONE_DEMO_SCREEN_BRIT_BUTTON_COLOR                   LCD_RED
#define MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_COLOR                  LCD_BLUE
#define MUPHONE_DEMO_SCREEN_INTRO_TEXT_COLOR                    LCD_WHITE
#define MUPHONE_DEMO_SCREEN_BUTTON_TEXT_COLOR                   LCD_WHITE

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

/* boundaries of phone name text in header bar */

#define MUPHONE_HEADER_BAR_NAME_PANEL_OFFSET            2       // spacing used for edges of button

#define MUPHONE_HEADER_BAR_NAME_BRIT_PANEL_WIDTH        ((LCD_TEXT_WIDTH * 12) + (MUPHONE_HEADER_BAR_NAME_PANEL_OFFSET * 2))
#define MUPHONE_HEADER_BAR_NAME_BRIT_PANEL_X_MIN             (MUPHONE_HEADER_BAR_X_MAX - MUPHONE_HEADER_BAR_NAME_BRIT_PANEL_WIDTH)
#define MUPHONE_HEADER_BAR_NAME_BRIT_PANEL_X_MAX             MUPHONE_HEADER_BAR_X_MAX
#define MUPHONE_HEADER_BAR_NAME_BRIT_PANEL_Y_MIN             MUPHONE_HEADER_BAR_Y_MIN
#define MUPHONE_HEADER_BAR_NAME_BRIT_PANEL_Y_MAX             (MUPHONE_HEADER_BAR_MESSAGE_NOTIFICATION_PANEL_Y_MIN + LCD_TEXT_HEIGHT + MUPHONE_HEADER_BAR_MESSAGE_NOTIFICATION_PANEL_OFFSET)

#define MUPHONE_HEADER_BAR_NAME_CHRIS_PANEL_WIDTH        ((LCD_TEXT_WIDTH * 11) + (MUPHONE_HEADER_BAR_NAME_PANEL_OFFSET * 2))
#define MUPHONE_HEADER_BAR_NAME_CHRIS_PANEL_X_MIN        (MUPHONE_HEADER_BAR_X_MAX - MUPHONE_HEADER_BAR_NAME_CHRIS_PANEL_WIDTH)
#define MUPHONE_HEADER_BAR_NAME_CHRIS_PANEL_X_MAX        MUPHONE_HEADER_BAR_X_MAX
#define MUPHONE_HEADER_BAR_NAME_CHRIS_PANEL_Y_MIN        MUPHONE_HEADER_BAR_Y_MIN
#define MUPHONE_HEADER_BAR_NAME_CHRIS_PANEL_Y_MAX        (MUPHONE_HEADER_BAR_MESSAGE_NOTIFICATION_PANEL_Y_MIN + LCD_TEXT_HEIGHT + MUPHONE_HEADER_BAR_MESSAGE_NOTIFICATION_PANEL_OFFSET)

#define MUPHONE_HEADER_BAR_NAME_TEXT_X_OFFSET       2
#define MUPHONE_HEADER_BAR_NAME_TEXT_Y_OFFSET       2

#define MUPHONE_HEADER_BAR_NAME_BRIT_TEXT_X_START        (MUPHONE_HEADER_BAR_NAME_BRIT_PANEL_X_MIN + MUPHONE_HEADER_BAR_NAME_TEXT_X_OFFSET)
#define MUPHONE_HEADER_BAR_NAME_BRIT_TEXT_Y_START        (MUPHONE_HEADER_BAR_NAME_BRIT_PANEL_Y_MIN + MUPHONE_HEADER_BAR_NAME_TEXT_Y_OFFSET)

#define MUPHONE_HEADER_BAR_NAME_CHRIS_TEXT_X_START        (MUPHONE_HEADER_BAR_NAME_CHRIS_PANEL_X_MIN + MUPHONE_HEADER_BAR_NAME_TEXT_X_OFFSET)
#define MUPHONE_HEADER_BAR_NAME_CHRIS_TEXT_Y_START        (MUPHONE_HEADER_BAR_NAME_CHRIS_PANEL_Y_MIN + MUPHONE_HEADER_BAR_NAME_TEXT_Y_OFFSET)

//#define MUPHONE_HEADER_BAR_CLOCK_PANEL_OFFSET      2       // spacing used for edges of button
//#define MUPHONE_HEADER_BAR_CLOCK_PANEL_WIDTH       ((LCD_TEXT_WIDTH * 18) + (MUPHONE_HEADER_BAR_CLOCK_PANEL_OFFSET * 2))
//#define MUPHONE_HEADER_BAR_CLOCK_PANEL_X_MIN       (MUPHONE_HEADER_BAR_X_MAX - MUPHONE_HEADER_BAR_CLOCK_PANEL_WIDTH)
//#define MUPHONE_HEADER_BAR_CLOCK_PANEL_X_MAX       MUPHONE_HEADER_BAR_X_MAX
//#define MUPHONE_HEADER_BAR_CLOCK_PANEL_Y_MIN       MUPHONE_HEADER_BAR_Y_MIN
//#define MUPHONE_HEADER_BAR_CLOCK_PANEL_Y_MAX       (MUPHONE_HEADER_BAR_MESSAGE_NOTIFICATION_PANEL_Y_MIN + LCD_TEXT_HEIGHT + MUPHONE_HEADER_BAR_MESSAGE_NOTIFICATION_PANEL_OFFSET)
//
//#define MUPHONE_HEADER_BAR_CLOCK_TEXT_X_OFFSET       2
//#define MUPHONE_HEADER_BAR_CLOCK_TEXT_Y_OFFSET       2
//#define MUPHONE_HEADER_BAR_CLOCK_TEXT_X_START        (MUPHONE_HEADER_BAR_CLOCK_PANEL_X_MIN + MUPHONE_HEADER_BAR_CLOCK_TEXT_X_OFFSET)
//#define MUPHONE_HEADER_BAR_CLOCK_TEXT_Y_START        (MUPHONE_HEADER_BAR_CLOCK_PANEL_Y_MIN + MUPHONE_HEADER_BAR_CLOCK_TEXT_Y_OFFSET)

// boundaries for MuPhone intro screen
#define MUPHONE_DEMO_SCREEN_X_MIN                   LCD_SCREEN_X_MIN
#define MUPHONE_DEMO_SCREEN_X_MAX                   LCD_SCREEN_X_MAX
#define MUPHONE_DEMO_SCREEN_Y_MIN                   LCD_SCREEN_Y_MIN
#define MUPHONE_DEMO_SCREEN_Y_MAX                   LCD_SCREEN_Y_MAX

#define MUPHONE_DEMO_SCREEN_INTRO_TEXT_X_START      25
#define MUPHONE_DEMO_SCREEN_INTRO_TEXT_Y_START      MUPHONE_DEMO_SCREEN_Y_MIN+22

#define DEMO_SCREEN_BUTTON_X_OFFSET                             60
#define DEMO_SCREEN_BUTTON_TEXT_Y_OFFSET                        10
#define DEMO_SCREEN_BUTTON_WIDTH                                120
#define DEMO_SCREEN_BUTTON_HEIGHT                               40

#define MUPHONE_DEMO_SCREEN_BRIT_BUTTON_X_MIN                   (((MUPHONE_DEMO_SCREEN_X_MAX - MUPHONE_DEMO_SCREEN_X_MIN) >> 1) - DEMO_SCREEN_BUTTON_X_OFFSET)
#define MUPHONE_DEMO_SCREEN_BRIT_BUTTON_X_MAX                   (MUPHONE_DEMO_SCREEN_BRIT_BUTTON_X_MIN + DEMO_SCREEN_BUTTON_WIDTH)
#define MUPHONE_DEMO_SCREEN_BRIT_BUTTON_Y_MIN                   90
#define MUPHONE_DEMO_SCREEN_BRIT_BUTTON_Y_MAX                   (MUPHONE_DEMO_SCREEN_BRIT_BUTTON_Y_MIN + DEMO_SCREEN_BUTTON_HEIGHT)

#define MUPHONE_DEMO_SCREEN_BRIT_BUTTON_TEXT_X_START            (MUPHONE_DEMO_SCREEN_BRIT_BUTTON_X_MIN + ((DEMO_SCREEN_BUTTON_WIDTH - (LCD_TEXT_WIDTH * 4)) >> 1))
#define MUPHONE_DEMO_SCREEN_BRIT_BUTTON_TEXT_Y_START            (MUPHONE_DEMO_SCREEN_BRIT_BUTTON_Y_MIN + DEMO_SCREEN_BUTTON_TEXT_Y_OFFSET)

#define MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_X_MIN                   (((MUPHONE_DEMO_SCREEN_X_MAX - MUPHONE_DEMO_SCREEN_X_MIN) >> 1) - DEMO_SCREEN_BUTTON_X_OFFSET)
#define MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_X_MAX                   (MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_X_MIN + DEMO_SCREEN_BUTTON_WIDTH)
#define MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_Y_MIN                   140
#define MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_Y_MAX                   (MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_Y_MIN + DEMO_SCREEN_BUTTON_HEIGHT)

#define MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_TEXT_X_START            (MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_X_MIN + ((DEMO_SCREEN_BUTTON_WIDTH - (LCD_TEXT_WIDTH * 5)) >> 1))
#define MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_TEXT_Y_START            (MUPHONE_DEMO_SCREEN_CHRIS_BUTTON_Y_MIN + DEMO_SCREEN_BUTTON_TEXT_Y_OFFSET)

// boundaries for MuPhone home screen
#define MUPHONE_HOME_SCREEN_WIDTH                   (LCD_SCREEN_X_MAX - LCD_SCREEN_X_MIN)
#define MUPHONE_HOME_SCREEN_HEIGHT                  (LCD_SCREEN_Y_MAX - MUPHONE_HEADER_BAR_DIVIDER_Y_MAX)
#define MUPHONE_HOME_SCREEN_X_MIN                   LCD_SCREEN_X_MIN
#define MUPHONE_HOME_SCREEN_X_MAX                   (LCD_SCREEN_X_MIN + MUPHONE_HOME_SCREEN_WIDTH)
#define MUPHONE_HOME_SCREEN_Y_MIN                   (MUPHONE_HEADER_BAR_DIVIDER_Y_MAX + 1)
#define MUPHONE_HOME_SCREEN_Y_MAX                   (MUPHONE_HOME_SCREEN_Y_MIN + MUPHONE_HOME_SCREEN_HEIGHT)

// boundaries for MuPhone application icons on home screen

#define MUPHONE_APP_ICON_X_EDGE_OFFSET              20      // left and right screen offset from edge of home-screen
#define MUPHONE_APP_ICON_Y_EDGE_OFFSET              10      // top and bottom screen offset from edge of home-screen
#define MUPHONE_APP_ICON_X_OFFSET                   20      // spacing between each application icon on a row of the home-screen
#define MUPHONE_APP_ICON_Y_OFFSET                   20
#define MUPHONE_APP_ICON_WIDTH                      55
#define MUPHONE_APP_ICON_HEIGHT                     55

#define MUPHONE_APP_ICON_BORDER_OFFSET              1

#define MUPHONE_APP_ICON_TEXT_Y_OFFSET              5

#define MUPHONE_APP_ICON_MUMESSAGE_X_MIN            (MUPHONE_HOME_SCREEN_X_MIN + (MUPHONE_APP_ICON_X_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 0) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 0) + (MUPHONE_APP_ICON_HEIGHT * 0))
#define MUPHONE_APP_ICON_MUMESSAGE_X_MAX            (MUPHONE_HOME_SCREEN_X_MIN + (MUPHONE_APP_ICON_X_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 0) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 1) + (MUPHONE_APP_ICON_HEIGHT * 0))
#define MUPHONE_APP_ICON_MUMESSAGE_Y_MIN            (MUPHONE_HOME_SCREEN_Y_MIN + (MUPHONE_APP_ICON_Y_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 0) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 0) + (MUPHONE_APP_ICON_HEIGHT * 0))
#define MUPHONE_APP_ICON_MUMESSAGE_Y_MAX            (MUPHONE_HOME_SCREEN_Y_MIN + (MUPHONE_APP_ICON_Y_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 0) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 0) + (MUPHONE_APP_ICON_HEIGHT * 1))

#define MUPHONE_APP_ICON_BORDER_MUMESSAGE_X_MIN     (MUPHONE_APP_ICON_MUMESSAGE_X_MIN - MUPHONE_APP_ICON_BORDER_OFFSET)
#define MUPHONE_APP_ICON_BORDER_MUMESSAGE_X_MAX     (MUPHONE_APP_ICON_MUMESSAGE_X_MAX + MUPHONE_APP_ICON_BORDER_OFFSET)
#define MUPHONE_APP_ICON_BORDER_MUMESSAGE_Y_MIN     (MUPHONE_APP_ICON_MUMESSAGE_Y_MIN - MUPHONE_APP_ICON_BORDER_OFFSET)
#define MUPHONE_APP_ICON_BORDER_MUMESSAGE_Y_MAX     (MUPHONE_APP_ICON_MUMESSAGE_Y_MAX + MUPHONE_APP_ICON_BORDER_OFFSET)

#define MUPHONE_APP_ICON_TEXT_MUMESSAGE_X_START     (MUPHONE_APP_ICON_MUMESSAGE_X_MIN + ((MUPHONE_APP_ICON_WIDTH - (LCD_TEXT_WIDTH * 9)) >> 1))
#define MUPHONE_APP_ICON_TEXT_MUMESSAGE_Y_START     (MUPHONE_APP_ICON_MUMESSAGE_Y_MAX + MUPHONE_APP_ICON_TEXT_Y_OFFSET)

#define MUPHONE_APP_ICON_BALLSY_X_MIN            (MUPHONE_HOME_SCREEN_X_MIN + (MUPHONE_APP_ICON_X_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 1) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 1) + (MUPHONE_APP_ICON_HEIGHT * 0))
#define MUPHONE_APP_ICON_BALLSY_X_MAX            (MUPHONE_HOME_SCREEN_X_MIN + (MUPHONE_APP_ICON_X_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 1) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 2) + (MUPHONE_APP_ICON_HEIGHT * 0))
#define MUPHONE_APP_ICON_BALLSY_Y_MIN            (MUPHONE_HOME_SCREEN_Y_MIN + (MUPHONE_APP_ICON_Y_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 0) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 0) + (MUPHONE_APP_ICON_HEIGHT * 0))
#define MUPHONE_APP_ICON_BALLSY_Y_MAX            (MUPHONE_HOME_SCREEN_Y_MIN + (MUPHONE_APP_ICON_Y_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 0) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 0) + (MUPHONE_APP_ICON_HEIGHT * 1))

#define MUPHONE_APP_ICON_BORDER_BALLSY_X_MIN     (MUPHONE_APP_ICON_BALLSY_X_MIN - MUPHONE_APP_ICON_BORDER_OFFSET)
#define MUPHONE_APP_ICON_BORDER_BALLSY_X_MAX     (MUPHONE_APP_ICON_BALLSY_X_MAX + MUPHONE_APP_ICON_BORDER_OFFSET)
#define MUPHONE_APP_ICON_BORDER_BALLSY_Y_MIN     (MUPHONE_APP_ICON_BALLSY_Y_MIN - MUPHONE_APP_ICON_BORDER_OFFSET)
#define MUPHONE_APP_ICON_BORDER_BALLSY_Y_MAX     (MUPHONE_APP_ICON_BALLSY_Y_MAX + MUPHONE_APP_ICON_BORDER_OFFSET)

#define MUPHONE_APP_ICON_TEXT_BALLSY_X_START     (MUPHONE_APP_ICON_BALLSY_X_MIN + ((MUPHONE_APP_ICON_WIDTH - (LCD_TEXT_WIDTH * 6)) >> 1))
#define MUPHONE_APP_ICON_TEXT_BALLSY_Y_START     (MUPHONE_APP_ICON_BALLSY_Y_MAX + MUPHONE_APP_ICON_TEXT_Y_OFFSET)

#define MUPHONE_APP_ICON_PONG_X_MIN            (MUPHONE_HOME_SCREEN_X_MIN + (MUPHONE_APP_ICON_X_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 2) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 2) + (MUPHONE_APP_ICON_HEIGHT * 0))
#define MUPHONE_APP_ICON_PONG_X_MAX            (MUPHONE_HOME_SCREEN_X_MIN + (MUPHONE_APP_ICON_X_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 2) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 3) + (MUPHONE_APP_ICON_HEIGHT * 0))
#define MUPHONE_APP_ICON_PONG_Y_MIN            (MUPHONE_HOME_SCREEN_Y_MIN + (MUPHONE_APP_ICON_Y_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 0) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 0) + (MUPHONE_APP_ICON_HEIGHT * 0))
#define MUPHONE_APP_ICON_PONG_Y_MAX            (MUPHONE_HOME_SCREEN_Y_MIN + (MUPHONE_APP_ICON_Y_EDGE_OFFSET * 1) + (MUPHONE_APP_ICON_X_OFFSET * 0) + (MUPHONE_APP_ICON_Y_OFFSET * 0) + (MUPHONE_APP_ICON_WIDTH * 0) + (MUPHONE_APP_ICON_HEIGHT * 1))

#define MUPHONE_APP_ICON_BORDER_PONG_X_MIN     (MUPHONE_APP_ICON_PONG_X_MIN - MUPHONE_APP_ICON_BORDER_OFFSET)
#define MUPHONE_APP_ICON_BORDER_PONG_X_MAX     (MUPHONE_APP_ICON_PONG_X_MAX + MUPHONE_APP_ICON_BORDER_OFFSET)
#define MUPHONE_APP_ICON_BORDER_PONG_Y_MIN     (MUPHONE_APP_ICON_PONG_Y_MIN - MUPHONE_APP_ICON_BORDER_OFFSET)
#define MUPHONE_APP_ICON_BORDER_PONG_Y_MAX     (MUPHONE_APP_ICON_PONG_Y_MAX + MUPHONE_APP_ICON_BORDER_OFFSET)

#define MUPHONE_APP_ICON_TEXT_PONG_X_START     (MUPHONE_APP_ICON_PONG_X_MIN + ((MUPHONE_APP_ICON_WIDTH - (LCD_TEXT_WIDTH * 4)) >> 1))
#define MUPHONE_APP_ICON_TEXT_PONG_Y_START     (MUPHONE_APP_ICON_PONG_Y_MAX + MUPHONE_APP_ICON_TEXT_Y_OFFSET)

////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////
//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//enum for threads to kill when a back button is pressed or the home button is pressed
//used for indexing an array of threads to kill

//NEED TO REINITIALIZE SEMAPHORES FOR THE THREADS THAT DIE
typedef enum{
    //for mumessage app
    thread_mumessage_messagelog = 10,
    //for ballsy app


}threads_to_kill_t;


typedef enum{

    thread_blink_cursor_enum = 0,

}periodic_threads_to_kill_t;

//corresponding array of TID's for the threads that need to die
tid_t threads_to_kill[11]; //unfortunately need to hardcode this, will automatically be set to zeros
pid_t periodic_threads_to_kill[1];


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


inline void kill_ballsy_threads();

inline void kill_mumessage_threads();

//////////////////////////END OF PUBLIC FUNCTION PROTOTYPES//////////////////////////

////////////////////////////////////THREADS//////////////////////////////////////////

/****************************** APERIODIC THREADS **********************************/

/************************************************************************************
 * Name: aperiodic_muphone_demo_screen
 * Purpose: Aperiodic event created after the initial display of the intro screen
 *          screen upon MuPhone boot-up. If a valid touch was made to the LCD TouchPanel,
 *          a thread for checking what section of the screen was pressed will be
 *          created.
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_muphone_demo_screen(void);

/************************************************************************************
 * Name: aperiodic_muphone_home_button
 * Purpose: Aperiodic event to register home button push (bottom daughter board button)
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_muphone_home_button(void);

/************************************************************************************
 * Name: aperiodic_muphone_home_screen
 * Purpose: Aperiodic event created after the initial display of the Compose Message
 *          screen within MuMessage. If a valid touch was made to the LCD TouchPanel,
 *          a thread for checking what section of the screen was pressed will be
 *          created.
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_muphone_home_screen(void);


/************************** END OF APERIODIC THREADS *******************************/


/******************************* COMMON THREADS ************************************/

/************************************************************************************
* Name: thread_muphone_demo_screen_check_TP
* Purpose: Thread to check if touch made to LCD TouchPanel interacted with any
*          predefined sections on the demo screen of the MuPhone.
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_muphone_demo_screen_check_TP(void);

/************************************************************************************
* Name: thread_muphone_home_button_check
* Purpose: Thread to check if MuPhone home button was pressed
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_muphone_home_button_check(void);

/************************************************************************************
* Name: thread_muphone_home_screen_check_TP
* Purpose: Thread to check if touch made to LCD TouchPanel interacted with any
*          predefined sections on the home screen of the MuPhone.
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_muphone_home_screen_check_TP(void);

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

/************************************************************************************
* Name: thread_muphone_start_phone
* Purpose: Thread to handle one-time system initializations
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_muphone_start_phone(void);



/**************************** END OF COMMON THREADS ********************************/


/******************************** HOST THREADS *************************************/

/***************************** END OF HOST THREADS *********************************/


/******************************* CLIENT THREADS ************************************/

/**************************** END OF CLIENT THREADS ********************************/


/////////////////////////////////END OF THREADS//////////////////////////////////////

#endif      // end of header guard

////////////////////////////////END OF muphone.h////////////////////////////////////
