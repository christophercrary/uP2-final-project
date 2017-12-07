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
