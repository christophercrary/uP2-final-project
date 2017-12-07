/************************************************************************************
 *  File name: muphone.c
 *  Modified: 22 November 2017
 *  Author:  Christopher Crary
 *  Purpose: To define any necessary functions/miscellaneous things for the MuPhone
 *           smartphone device.
************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////

#include "muphone.h"
#include "mumessage.h"      // MuMessage messaging application

#include "pong.h"       // pong game application

// IMPLEMENT OTHER APPLICATIONS

/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////
//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

/////////////////////////////////PRIVATE FUNCTIONS///////////////////////////////////

/************************************************************************************
* Name: muphone_init
* Purpose: Helper function to initialize the MuPhone device.
* Input(s): N/A
* Output: N/A
************************************************************************************/
static inline void muphone_init(void)
{
    // initialize the MSP432 and the G8RTOS
    G8RTOS_init();

    // add the necessary starting threads
    G8RTOS_add_thread(thread_muphone_idle, 200, "idle");
    //G8RTOS_add_thread(thread_start_game, 180, "pong - start");      // TEST
    G8RTOS_add_thread(thread_mumessage_compose_message, 180, "compose message");

    // launch the operating system
    G8RTOS_launch();
}

//////////////////////////////END OF PRIVATE FUNCTIONS///////////////////////////////

////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////

/************************************************************************************
* Name: muphone
* Purpose: User function to initialize the MuPhone
* Input(s): N/A
* Output: N/A
************************************************************************************/
void muphone(void)
{
    /* initialize device hardware to allow MuPhone to be "turned on" */
    muphone_init();     // call to statically defined helper function

    return;     // never reached
}

/////////////////////////////END OF PUBLIC FUNCTIONS/////////////////////////////////

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
void thread_muphone_idle(void)
{
    while(1);
}

/************************************************************************************
* Name: thread_muphone_lock_screen
* Purpose: Thread to handle the MuPhone's lock screen
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_muphone_lock_screen(void)
{
    // IMPLEMENT
}

/************************************************************************************
* Name: thread_muphone_home_screen
* Purpose: Thread to handle the MuPhone's home screen
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_muphone_home_screen(void)
{
    // IMPLEMENT
}

/**************************** END OF COMMON THREADS ********************************/


/******************************** HOST THREADS *************************************/

/***************************** END OF HOST THREADS *********************************/


/******************************* CLIENT THREADS ************************************/

/**************************** END OF CLIENT THREADS ********************************/

/////////////////////////////////END OF THREADS//////////////////////////////////////



////////////////////////////INTERRUPT SERVICE ROUTINES///////////////////////////////
////////////////////////END OF INTERRUPT SERVICE ROUTINES////////////////////////////

////////////////////////////////END OF muphone.c////////////////////////////////////
