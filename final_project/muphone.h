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
