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
#include "Applications.h"
#include "pong.h"       // pong game application
#include "time.h"

// IMPLEMENT OTHER APPLICATIONS

/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
extern semaphore_t semaphore_CC3100;
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////

// global variables to store information about the MuPhone system time
// IMPLEMENT: MAKE PRIVATE TO MUPHONE (MUPHONE RESPONSIBLE FOR DISPLAYING TIME)
//time_t system_time;
//struct tm *timeinfo;

//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////

const static Rectangle section_muphone_header_bar[] =
{
     {MUPHONE_HEADER_BAR_X_MIN, MUPHONE_HEADER_BAR_X_MAX, MUPHONE_HEADER_BAR_Y_MIN,
      MUPHONE_HEADER_BAR_Y_MAX, MUPHONE_HEADER_BAR_COLOR},
     {MUPHONE_HEADER_BAR_DIVIDER_X_MIN, MUPHONE_HEADER_BAR_DIVIDER_X_MAX,
      MUPHONE_HEADER_BAR_DIVIDER_Y_MIN, MUPHONE_HEADER_BAR_DIVIDER_Y_MAX,
      MUPHONE_HEADER_BAR_DIVIDER_COLOR}
};

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
    /* initialize the MSP432 and the G8RTOS */
    G8RTOS_init();

    /* draw MuPhone visuals */

    LCD_DrawSection(section_muphone_header_bar,
                    (sizeof(section_muphone_header_bar)/sizeof(section_muphone_header_bar[0])));

//    LCD_PrintTextSection(text_section_muphone_header_bar,
//                    (sizeof(text_section_muphone_header_bar)/sizeof(text_section_muphone_header_bar[0])));

    /* add the necessary starting threads */
    G8RTOS_add_thread(thread_muphone_idle, 200, "idle");
    //G8RTOS_add_thread(thread_start_game, 180, "pong - start");      // TEST

    // MuMessage background processes
    G8RTOS_add_thread(thread_mumessage_background_processes, 150, "mumessage - b.p.");
    G8RTOS_add_thread(thread_mumessage_compose_message, 180, "compose message");

    G8RTOS_semaphore_init(&semaphore_CC3100, 1);

    phone.board_type = Host;
    //phone.board_type = Client;
    phone.current_app = HOME_SCREEN;
    mu_message.old_messages.number_of_strings = 0;
    //message data should already be initialized to zero

    if(phone.board_type == Host)
    {
      //  G8RTOS_add_thread(thread_init_host_wifi, 30, "initHostWifi"); CHRIS COMMENT, UNCOMMENT FOR WIFI APPLICATION
    }
    else
    {
        //client
      // G8RTOS_add_thread(thread_init_client_wifi, 30, "initClientWifi"); CHRIS COMMENT, UNCOMMENT FOR WIFI APPLICATION

       // G8RTOS_add_thread(thread_send_pong_data, 50, "sendPongData"); //client will currently be sending this stuff to the host

       // G8RTOS_add_thread(thread_send_message_data, 50, "sendMessageData");

    }

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
