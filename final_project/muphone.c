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

// IMPLEMENT OTHER APPLICATIONS

/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
extern semaphore_t semaphore_CC3100;
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

    G8RTOS_semaphore_init(&semaphore_CC3100, 1);

    phone.board_type = Host;
    //phone.board_type = Client;
    phone.current_app = HOME_SCREEN;


    //initializations for mu_message
    index_of_message_row = 0;
   // index_of_message_col = 0; //for data structure
    mu_message.old_messages.row_index = 0;
   // mu_message.old_messages.col_index = 0;
    mu_message.message_data.header_info.intended_app = MUMESSAGE;
    mu_message.message_data.header_info.size_of_data = 0; //initialize size to send to zero
    phone.message_received = 0;
    mu_message.old_messages.number_of_strings = 0;
    //message data should already be initialized to zero


    //initialize pong variables
    /* add aperiodic threads */
    P4->OUT = 0b00010000; //enable pull up resistor
    P4->REN = 0b00010000;

    /* initialize button press interrupt */
    P4->DIR &= ~BIT4; //P4 is an input
    P4->IFG &= ~BIT4;
    P4->IES |= BIT4; //set bit 4 for high to low transition
    P4->IE |= BIT4; //enable bit 4 interrupt
    leds_init();

    if(phone.board_type == Host)
    {
        G8RTOS_add_thread(thread_init_host_wifi, 30, "initHostWifi");
        phone.self_contact = BRIT;
    }
    else
    {
        //client
       G8RTOS_add_thread(thread_init_client_wifi, 30, "initClientWifi");
       phone.self_contact = CHRIS;
     //  phone.self_contact = WES;
    }


   // G8RTOS_add_thread(thread_start_game, 40, "Start Game");

    G8RTOS_add_thread(thread_receive_data, 40, "receiveData");

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

    LCD_DrawRectangle(MU_MESSAGE_APP_X_START,
                      MU_MESSAGE_APP_X_END,
                      MU_MESSAGE_APP_Y_START,
                      MU_MESSAGE_APP_Y_END,
                      LCD_BLUE);

    LCD_DrawRectangle( PONG_APP_X_START,
                       PONG_APP_X_END,
                       PONG_APP_Y_START,
                       PONG_APP_Y_END,
                       LCD_BLUE);


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
