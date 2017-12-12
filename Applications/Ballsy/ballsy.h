/************************************************************************************
 *  File name: ballsy.h
 *  Modified: 13 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To declare useful threads for the G8RTOS system.
 ************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////
#include "G8RTOS_structures.h"  // used for bool_t and tid_t datatypes
/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef BALLSY_H_
#define BALLSY_H_


#define MAX_BALLS       20      // max number of balls allowed to be created

#define FIFO_coordinates    0      // first FIFO declared in FIFO list
                                   // the FIFO functions are garbage

/* boundaries of message log text arena */
#define BALLSY_GAME_ARENA_X_MIN        LCD_SCREEN_X_MIN
#define BALLSY_GAME_ARENA_X_MAX        LCD_SCREEN_X_MAX
#define BALLSY_GAME_ARENA_Y_MIN        (MUPHONE_HEADER_BAR_DIVIDER_Y_MAX + 1)
#define BALLSY_GAME_ARENA_Y_MAX        LCD_SCREEN_Y_MAX

#define BALLSY_GAME_ARENA_COLOR        LCD_BLACK

////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////

////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////
typedef struct ball{
    struct ball *next;          // used for static ball linked list
    struct ball *prev;          // used for static ball linked list
    int16_t x_coordinate;       // x-coordinate of center of 4x4 ball
    int16_t y_coordinate;       // y-coordinate of center of 4x4 ball
    int8_t x_velocity;         // velocity of the x-coordinate,
                                //based on the accelerometer data
    int8_t y_velocity;         // velocity of the x-coordinate,
                                //based on the accelerometer data
    bool isAlive;             // used to determine availability in static ball LL
    uint16_t color;             // random uint16_t value assigned to determine color
    tid_t thread_id;            // thread ID based on specific ball thread
}ball_t;

//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////

////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////


////////////////////////////PUBLIC FUNCTION PROTOTYPES///////////////////////////////

/************************************************************************************
 * Name: aperiodic_port4
 * Purpose: Aperiodic event to register up push-button and right push-button
 *          presses on daughter board. The LCD TouchPanel is also handled in this
 *          aperiodic thread.
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_port4(void);

/************************************************************************************
 * Name: aperiodic_port5
 * Purpose: Aperiodic event to register left push-button and down push-button
 *          presses on daughter board.
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_port5(void);

/************************************************************************************
 * Name: pet_joystick
 * Purpose: Read joystick's x-axis data, and then write this data to the joystick
 *          FIFO. Additionally, this PET toggles P3, pin 3 on the MSP432.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void pet_joystick(void);

/************************************************************************************
 * Name: pet_test1
 * Purpose: used to test dynamic PETS
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void pet_test1(void);

/************************************************************************************
 * Name: thread_ball
 * Purpose: High priority thread created from touch being made to LCD Touch Panel.
 *          This thread creates a new thread_create_ball thread, which will create
 *          and display a 4x4 ball of a random RGB color
 *          on the LCD display screen, based on the location of the user's press.
 *          This thread will then update the ball's location on the LCD screen every
 *          30 milliseconds (~30 frames per second), and then sleep.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_ball(void);

/************************************************************************************
 * Name: thread_ball_program_init
 * Purpose: High priority thread to initialize the balls static array "linked list"
 *          (initialize all balls to being "dead"), to initialize the ball counter,
 *          and the respective head and tail pointers of the ball "linked list".
 *          After conducting the appropriate initializations, the thread kills itself.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_ball_program_init(void);

/************************************************************************************
 * Name: thread_debounce_LCD
 * Purpose: High priority thread created from touch being made to LCD Touch Panel.
 *          Before entering this thread, the LCD's Touch Panel interrupt on
 *          port 4, pin 0 was disabled in the aperiodic thread aperiodic_port4.
 *          This thread creates a new thread_ball thread, which will create
 *          and display a 4x4 ball of a random RGB color
 *          on the LCD display screen, based on the location of the user's press.
 *          After creating the other thread, this thread will sleep for 500ms, as to
 *          debounce the LCD Touch Panel. After the 500ms slumber, the Touch Panel
 *          interrupt on port 4, pin 0 will be rearmed, and the proper interrupt flag
 *          will be cleared. Finally, this thread will
 *          kill itself, as to allow the LCD touch panel screen to be pressed again,
 *          and to allow other balls to be created.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_debounce_LCD(void);

/************************************************************************************
 * Name: thread_idle
 * Purpose: To prevent infinite loops for situations where there are only sleeping/
 *          blocked threads.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_idle(void);


//////////////////////////END OF PUBLIC FUNCTION PROTOTYPES//////////////////////////

#endif      // end of header guard

///////////////////////////END OF threads.h///////////////////////////////////
