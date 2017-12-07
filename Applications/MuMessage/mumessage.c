/************************************************************************************
 *  File name: mumessage.c
 *  Modified: 22 November 2017
 *  Author:  Christopher Crary
 *  Purpose: Source file for MuMessage, a built-in messaging application for MuPhone.
************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////
#include "mumessage.h"
#include "G8RTOS.h"
#include "LCD.h"     // QUESTION: what is the best way to include LCD.h?
#include "GPIO.h"    // QUESTION: what is the best way to include GPIO.h?

/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////

/* MISCELLANEOUS MUMESSAGE DEFINES */

////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////

//test (REMOVE)
Point touch;

// static cursor to be used in any typing environment
static Point cursor;

//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////

/************************** COMPOSE MESSAGE MEMBERS ********************************/

/* sections used to draw Compose Message and used to check for touches made
 * to the LCD TouchPanel */

// compose message background section
static Rectangle section_compose_message_background[] =
{
    {COMPOSE_MESSAGE_BACKGROUND_X_MIN, COMPOSE_MESSAGE_BACKGROUND_X_MAX,
     COMPOSE_MESSAGE_BACKGROUND_Y_MIN, COMPOSE_MESSAGE_BACKGROUND_Y_MAX,
     COMPOSE_MESSAGE_BACKGROUND_COLOR}
};

// compose message header bar
static Rectangle section_compose_message_header[] =
{
    {COMPOSE_MESSAGE_HEADER_X_MIN, COMPOSE_MESSAGE_HEADER_X_MAX,
     COMPOSE_MESSAGE_HEADER_Y_MIN, COMPOSE_MESSAGE_HEADER_Y_MAX,
     COMPOSE_MESSAGE_HEADER_COLOR},
    {COMPOSE_MESSAGE_HEADER_DIVIDER_X_MIN, COMPOSE_MESSAGE_HEADER_DIVIDER_X_MAX,
     COMPOSE_MESSAGE_HEADER_DIVIDER_Y_MIN, COMPOSE_MESSAGE_HEADER_DIVIDER_Y_MAX,
     COMPOSE_MESSAGE_HEADER_DIVIDER_COLOR}
};

// compose message header bar buttons (back/send)
static Rectangle section_compose_message_header_buttons[] =
{
     {COMPOSE_MESSAGE_BACK_BUTTON_X_MIN, COMPOSE_MESSAGE_BACK_BUTTON_X_MAX,
      COMPOSE_MESSAGE_BACK_BUTTON_Y_MIN, COMPOSE_MESSAGE_BACK_BUTTON_Y_MAX,
      COMPOSE_MESSAGE_BACK_BUTTON_COLOR},
     {COMPOSE_MESSAGE_SEND_BUTTON_X_MIN, COMPOSE_MESSAGE_SEND_BUTTON_X_MAX,
      COMPOSE_MESSAGE_SEND_BUTTON_Y_MIN, COMPOSE_MESSAGE_SEND_BUTTON_Y_MAX,
      COMPOSE_MESSAGE_SEND_BUTTON_COLOR}
};

// compose message header buttons text
static Text text_compose_message_header[] =
{
     {COMPOSE_MESSAGE_BACK_BUTTON_TEXT_X_START, COMPOSE_MESSAGE_BACK_BUTTON_TEXT_Y_START,
      "< BACK", COMPOSE_MESSAGE_BACK_BUTTON_TEXT_COLOR},
     {COMPOSE_MESSAGE_SEND_BUTTON_TEXT_X_START, COMPOSE_MESSAGE_SEND_BUTTON_TEXT_Y_START,
      "SEND MESSAGE", COMPOSE_MESSAGE_SEND_BUTTON_TEXT_COLOR}
};

// text arena background section
static Rectangle section_text_arena[] =
{
     {TEXT_ARENA_X_MIN, TEXT_ARENA_X_MAX, TEXT_ARENA_Y_MIN, TEXT_ARENA_Y_MAX, COMPOSE_MESSAGE_TEXT_ARENA_COLOR}
};

// keyboard background section
static Rectangle section_keyboard_background[] =
{
    {KEYBOARD_DIVIDER_X_MIN, KEYBOARD_DIVIDER_X_MAX,
     KEYBOARD_DIVIDER_Y_MIN, KEYBOARD_DIVIDER_Y_MAX,
     KEYBOARD_DIVIDER_COLOR},
    {KEYBOARD_X_MIN, KEYBOARD_X_MAX, KEYBOARD_Y_MIN, KEYBOARD_Y_MAX, KEYBOARD_COLOR}
};

// keyboard #1 (lower-case keyboard)
static Rectangle section_keyboard1[] =
{
     {Q_BUTTON_X_MIN, Q_BUTTON_X_MAX, Q_BUTTON_Y_MIN, Q_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},                                        // row 1 of 4
     {W_BUTTON_X_MIN, W_BUTTON_X_MAX, W_BUTTON_Y_MIN, W_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {E_BUTTON_X_MIN, E_BUTTON_X_MAX, E_BUTTON_Y_MIN, E_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {R_BUTTON_X_MIN, R_BUTTON_X_MAX, R_BUTTON_Y_MIN, R_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {T_BUTTON_X_MIN, T_BUTTON_X_MAX, T_BUTTON_Y_MIN, T_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {Y_BUTTON_X_MIN, Y_BUTTON_X_MAX, Y_BUTTON_Y_MIN, Y_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {U_BUTTON_X_MIN, U_BUTTON_X_MAX, U_BUTTON_Y_MIN, U_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {I_BUTTON_X_MIN, I_BUTTON_X_MAX, I_BUTTON_Y_MIN, I_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {O_BUTTON_X_MIN, O_BUTTON_X_MAX, O_BUTTON_Y_MIN, O_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {P_BUTTON_X_MIN, P_BUTTON_X_MAX, P_BUTTON_Y_MIN, P_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {A_BUTTON_X_MIN, A_BUTTON_X_MAX, A_BUTTON_Y_MIN, A_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},                                        // row 2 of 4
     {S_BUTTON_X_MIN, S_BUTTON_X_MAX, S_BUTTON_Y_MIN, S_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {D_BUTTON_X_MIN, D_BUTTON_X_MAX, D_BUTTON_Y_MIN, D_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {F_BUTTON_X_MIN, F_BUTTON_X_MAX, F_BUTTON_Y_MIN, F_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {G_BUTTON_X_MIN, G_BUTTON_X_MAX, G_BUTTON_Y_MIN, G_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {H_BUTTON_X_MIN, H_BUTTON_X_MAX, H_BUTTON_Y_MIN, H_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {J_BUTTON_X_MIN, J_BUTTON_X_MAX, J_BUTTON_Y_MIN, J_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {K_BUTTON_X_MIN, K_BUTTON_X_MAX, K_BUTTON_Y_MIN, K_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {L_BUTTON_X_MIN, L_BUTTON_X_MAX, L_BUTTON_Y_MIN, L_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {UPPERCASE_BUTTON_X_MIN, UPPERCASE_BUTTON_X_MAX, UPPERCASE_BUTTON_Y_MIN, UPPERCASE_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},        // row 3 of 4
     {Z_BUTTON_X_MIN, Z_BUTTON_X_MAX, Z_BUTTON_Y_MIN, Z_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {X_BUTTON_X_MIN, X_BUTTON_X_MAX, X_BUTTON_Y_MIN, X_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {C_BUTTON_X_MIN, C_BUTTON_X_MAX, C_BUTTON_Y_MIN, C_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {V_BUTTON_X_MIN, V_BUTTON_X_MAX, V_BUTTON_Y_MIN, V_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {B_BUTTON_X_MIN, B_BUTTON_X_MAX, B_BUTTON_Y_MIN, B_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {N_BUTTON_X_MIN, N_BUTTON_X_MAX, N_BUTTON_Y_MIN, N_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {M_BUTTON_X_MIN, M_BUTTON_X_MAX, M_BUTTON_Y_MIN, M_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {BACKSPACE_BUTTON_X_MIN, BACKSPACE_BUTTON_X_MAX, BACKSPACE_BUTTON_Y_MIN, BACKSPACE_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {NUMBERS_BUTTON_X_MIN, NUMBERS_BUTTON_X_MAX, NUMBERS_BUTTON_Y_MIN, NUMBERS_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},                // row 4 of 4
     {SPACE_BUTTON_X_MIN, SPACE_BUTTON_X_MAX, SPACE_BUTTON_Y_MIN, SPACE_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR},
     {RETURN_BUTTON_X_MIN, RETURN_BUTTON_X_MAX, RETURN_BUTTON_Y_MIN, RETURN_BUTTON_Y_MAX, KEYBOARD_KEY_NO_CLICK_COLOR}
};

// keyboard #1 text
static Text text_keyboard1[] =
{
     {Q_BUTTON_TEXT_X_START, Q_BUTTON_TEXT_Y_START, "Q", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},                                        // row 1 of 4
     {W_BUTTON_TEXT_X_START, W_BUTTON_TEXT_Y_START, "W", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {E_BUTTON_TEXT_X_START, E_BUTTON_TEXT_Y_START, "E", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {R_BUTTON_TEXT_X_START, R_BUTTON_TEXT_Y_START, "R", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {T_BUTTON_TEXT_X_START, T_BUTTON_TEXT_Y_START, "T", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {Y_BUTTON_TEXT_X_START, Y_BUTTON_TEXT_Y_START, "Y", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {U_BUTTON_TEXT_X_START, U_BUTTON_TEXT_Y_START, "U", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {I_BUTTON_TEXT_X_START, I_BUTTON_TEXT_Y_START, "I", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {O_BUTTON_TEXT_X_START, O_BUTTON_TEXT_Y_START, "O", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {P_BUTTON_TEXT_X_START, P_BUTTON_TEXT_Y_START, "P", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {A_BUTTON_TEXT_X_START, A_BUTTON_TEXT_Y_START, "A", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},                                        // row 2 of 4
     {S_BUTTON_TEXT_X_START, S_BUTTON_TEXT_Y_START, "S", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {D_BUTTON_TEXT_X_START, D_BUTTON_TEXT_Y_START, "D", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {F_BUTTON_TEXT_X_START, F_BUTTON_TEXT_Y_START, "F", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {G_BUTTON_TEXT_X_START, G_BUTTON_TEXT_Y_START, "G", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {H_BUTTON_TEXT_X_START, H_BUTTON_TEXT_Y_START, "H", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {J_BUTTON_TEXT_X_START, J_BUTTON_TEXT_Y_START, "J", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {K_BUTTON_TEXT_X_START, K_BUTTON_TEXT_Y_START, "K", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {L_BUTTON_TEXT_X_START, L_BUTTON_TEXT_Y_START, "L", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {UPPERCASE_BUTTON_TEXT_X_START, UPPERCASE_BUTTON_TEXT_Y_START, "~^|", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},        // row 3 of 4
     {Z_BUTTON_TEXT_X_START, Z_BUTTON_TEXT_Y_START, "Z", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {X_BUTTON_TEXT_X_START, X_BUTTON_TEXT_Y_START, "X", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {C_BUTTON_TEXT_X_START, C_BUTTON_TEXT_Y_START, "C", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {V_BUTTON_TEXT_X_START, V_BUTTON_TEXT_Y_START, "V", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {B_BUTTON_TEXT_X_START, B_BUTTON_TEXT_Y_START, "B", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {N_BUTTON_TEXT_X_START, N_BUTTON_TEXT_Y_START, "N", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {M_BUTTON_TEXT_X_START, M_BUTTON_TEXT_Y_START, "M", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {BACKSPACE_BUTTON_TEXT_X_START, BACKSPACE_BUTTON_TEXT_Y_START, "<X", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {NUMBERS_BUTTON_TEXT_X_START, NUMBERS_BUTTON_TEXT_Y_START, "123", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},                // row 4 of 4
     {SPACE_BUTTON_TEXT_X_START, SPACE_BUTTON_TEXT_Y_START, "SPACE", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR},
     {RETURN_BUTTON_TEXT_X_START, RETURN_BUTTON_TEXT_Y_START, "RETURN", KEYBOARD_KEY_TEXT_NO_CLICK_COLOR}
};

/* miscellaneous compose message defines */

// first of three char arrays used to represent the first of three keyboards
// when checking for touches made to keyboard
// NOTE: '^' is a filler used to determine when to change keyboard to upper-case keyboard
// NOTE: '1' is a filler used to determine when to change keyboard to numbers/symbols keyboard
// NOTE: ' ' is a filler used to determine when to add a space to the messgge
// NOTE: '|' is a filler used to determine when to create a new line
static char keyboard1_keys[31] =
{
 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p',
   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l',
   '^', 'z', 'x', 'c', 'v', 'b', 'n', 'm', '<',
                  '1', ' ', '|'
};


// array used to store current message being composed
static uint8_t current_message[512];

// index used to keep track of current character index in currently composed message
static uint16_t current_message_index = 0;

/************************* END OF COMPOSE MESSAGE MEMBERS **************************/

// global message log

//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

/////////////////////////////////PRIVATE FUNCTIONS///////////////////////////////////

/************************************************************************************
* Name: insert_character
* Purpose: Helper function to insert an ASCII character from a given cursor position
* Input(s): Point cursor, uint8_t character
* Output: N/A
************************************************************************************/
static inline void insert_character(uint8_t character)
{
    // IMPLEMENT: insert semaphore for cursor

    if ((cursor.x < COMPOSE_MESSAGE_CURSOR_X_MAX) &&
        (cursor.y < COMPOSE_MESSAGE_CURSOR_Y_MAX))
    {
        // the only way to successfully write back a character after deleting a character
        // fucking beats Brit and I (QUESTION)
        LCD_DrawRectangle(cursor.x, (cursor.x + LCD_TEXT_WIDTH), cursor.y,
                         (cursor.y + LCD_TEXT_HEIGHT), LCD_WHITE);

        LCD_PutChar(cursor.x, cursor.y, character, LCD_TEXT_COLOR);
        cursor.x += LCD_TEXT_WIDTH;
    }

    return;
}

/************************************************************************************
* Name: delete_character
* Purpose: Helper function to delete an ASCII character from a given cursor position
* Input(s): Point cursor
* Output: N/A
************************************************************************************/
static inline void delete_character()
{
    // IMPLEMENT: insert semaphore for cursor (for blinking)

    // check bounds for cursor
    if ((cursor.x > COMPOSE_MESSAGE_CURSOR_X_MIN) &&
        (cursor.y >= COMPOSE_MESSAGE_CURSOR_Y_MIN))
    {
    // erase previous character from text arena
    LCD_DrawRectangle((cursor.x - LCD_TEXT_WIDTH), cursor.x, cursor.y,
                      (cursor.y + LCD_TEXT_HEIGHT), LCD_WHITE);

    cursor.x -= LCD_TEXT_WIDTH;
    }


    return;
}

//////////////////////////////END OF PRIVATE FUNCTIONS///////////////////////////////

////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////

/************************************************************************************
* Name:
* Purpose:
* Input(s):
* Output:
************************************************************************************/

/////////////////////////////END OF PUBLIC FUNCTIONS/////////////////////////////////

////////////////////////////////////THREADS//////////////////////////////////////////

/****************************** APERIODIC THREADS **********************************/

/************************************************************************************
 * Name: aperiodic_mumessage_compose_message
 * Purpose: Aperiodic event created after the initial display of the Compose Message
 *          screen within MuMessage. If a valid touch was made to the LCD TouchPanel,
 *          a thread for checking what section of the screen was pressed will be
 *          created.
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_mumessage_compose_message(void)
{
    uint8_t status;     // used to identify which P4 pin triggered the ISR (pins 4 or 5)

    status = P4IV;      // P4IV = (2*(n+1)) where n is the pin number of the
                        // lowest bit with a pending interrupt.
                        // this access will only clear flag n.

    // if triggering switch is high after debounce time
    if ( ( P4->IN & ( 1 << (status/2) - 1 ) ) == ( 1 << (status/2) - 1 ) )
    {
        return;     // ISR improperly called
    }

    // otherwise, interrupt was properly called

    // if LCD TP triggered interrupt (port 4, pin 0)
    if (status == 2)        //  2 = 2*(0+2)
    {
        // disable pin 0 interrupt (will be rearmed after 0.5 seconds) (IMPLEMENT!)
        GPIO_disableInterrupt(GPIO_PORT_P4, GPIO_PIN0);

        // create thread to check LCD screen press within Compose Message
        G8RTOS_add_thread(thread_mumessage_compose_message_check_TP, 50, "TP - compose mess.");
    }
}

/**************************** END OF APERIODIC THREADS *****************************/


/******************************* COMMON THREADS ************************************/

/* threads for Compose Message */

/************************************************************************************
* Name: thread_mumessage_compose_message
* Purpose: Thread to draw Compose Message screen, and prepare for sending a
*          message to another MuPhone user.
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_mumessage_compose_message(void)
{
    /* draw Compose Message screen visuals */

    LCD_DrawSection(section_compose_message_background,
                    (sizeof(section_compose_message_background)/sizeof(section_compose_message_background[0])));

    LCD_DrawSection(section_compose_message_header,
                    (sizeof(section_compose_message_header)/sizeof(section_compose_message_header[0])));

    LCD_DrawSection(section_compose_message_header_buttons,
                    (sizeof(section_compose_message_header_buttons)/sizeof(section_compose_message_header_buttons[0])));

    LCD_PrintTextStructure(text_compose_message_header,
                           (sizeof(text_compose_message_header)/sizeof(text_compose_message_header[0])));

    LCD_DrawSection(section_text_arena,
                        (sizeof(section_text_arena)/sizeof(section_text_arena[0])));

    LCD_DrawSection(section_keyboard_background,
                    (sizeof(section_keyboard_background)/sizeof(section_keyboard_background[0])));

    LCD_DrawSection(section_keyboard1,
                    (sizeof(section_keyboard1)/sizeof(section_keyboard1[0])));

    LCD_PrintTextStructure(text_keyboard1,
                           (sizeof(text_keyboard1)/sizeof(text_keyboard1[0])));

    // initialize the global cursor to the beginning of the text arena
    cursor.x = COMPOSE_MESSAGE_CURSOR_X_MIN;
    cursor.y = COMPOSE_MESSAGE_CURSOR_Y_MIN;

    /* add the necessary aperiodic thread for touches made to the LCD TouchPanel */
    G8RTOS_add_aperiodic_thread(aperiodic_mumessage_compose_message, PORT4_IRQn, 6);

    G8RTOS_kill_current_thread();

}

/************************************************************************************
* Name: thread_mumessage_compose_message_check_TP
* Purpose: Thread to check if touch made to LCD TouchPanel interacted with any
*          predefined sections in the Compose Message screen of MuMessage.
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_mumessage_compose_message_check_TP(void)
{
    // record touch made to the LCD TouchPanel
    touch = TP_ReadXY();

    // indexing variable used to determine what rectangle within specified section
    // the touch interacted with (if none, index will equal -1)
    int index = -1;

    // check if touch interacted with back button (IMPLEMENT)

    // check if touch interacted with message box (IMPLEMENT)

    // check if touch interacted with 'Send' button (IMPLEMENT)

    // check if touch interacted with keyboard
    index = TP_CheckForSectionPress( touch, section_keyboard1,
                                     (sizeof(section_keyboard1)/sizeof(section_keyboard1[0])) );

    if (index != -1)
    {
        if (index == 27)
        {
            delete_character();
        }
        else
        {
            insert_character(keyboard1_keys[index]);
        }
    }

    // wait quarter of a second before re-enabling LCD TouchPanel interrupt
    // IMPLEMENT: SHORTER TIME BETWEEN KEYS AND BOOLEAN TO WAIT UNTIL KEY IS RELEASED
    G8RTOS_thread_sleep(QUARTER_SECOND_MS);

    // re-enable LCD Touch Panel interrupt and clear interrupt flag
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN0);

    // kill thread (rip)
    G8RTOS_kill_current_thread();
}

/************************************************************************************
* Name: thread_mumessage_check_for_messages
* Purpose: Background thread to check if new messages were received
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_mumessage_check_for_messages(void)
{
    // IMPLEMENT!!
}

/************************************************************************************
* Name: thread_mumessage_open_app
* Purpose: Thread to open already initialized MuMessage application instance
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_mumessage_open_app(void)
{
    // draw application visuals

    while(1)
    {

    }
}

/************************************************************************************
* Name: thread_mumessage_start_app
* Purpose: Thread to initialize MuMessage application as well as any necessary
*          application background processes
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_mumessage_start_app(void)
{

    // initialize application instance
}

/**************************** END OF COMMON THREADS ********************************/


/******************************** HOST THREADS *************************************/

/***************************** END OF HOST THREADS *********************************/


/******************************* CLIENT THREADS ************************************/

/**************************** END OF CLIENT THREADS ********************************/


/////////////////////////////////END OF THREADS//////////////////////////////////////



////////////////////////////INTERRUPT SERVICE ROUTINES///////////////////////////////
////////////////////////END OF INTERRUPT SERVICE ROUTINES////////////////////////////

////////////////////////////////END OF mumessage.c////////////////////////////////////
