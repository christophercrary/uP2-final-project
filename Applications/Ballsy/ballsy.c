/************************************************************************************
 *  File name: threads.c
 *  Modified: 22 October 2017
 *  Author:  Christopher Crary
 *  Purpose: To define useful threads for the G8RTOS system.
 ************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////
#include "ballsy.h"
#include "muphone.h"
#include <time.h>       // used to help seed a random number
#include <stdlib.h>     // used to seed a random number
#include <stdio.h>
/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////

// test counters
uint16_t counter0;
uint16_t counter1;
uint16_t counter2;
uint16_t counter3;

/* semaphores */
semaphore_t semaphore_ball_list;        // used for accessing static ball array ("linked list")
semaphore_t semaphore_coordinates;      // used for accessing global LCD Touch Panel coordinates
semaphore_t semaphore_LCD;              // used for accessing LCD
semaphore_t semaphore_ball_count;       // used for updating the amount of balls active
semaphore_t semaphore_ball_head;        // used to identify the head of the ball static LL

extern semaphore_t semaphore_CC3100;

/* FIFOs */
fifo_t fifo_coordinates;        // used for storing coordinates upon a touch being
                                // made to the LCD touch panel

// static global variable to store x and y coordinates of LCD Touch Panel press
static Point coordinates;

// static array of balls ("linked list")
static ball_t balls[MAX_BALLS];

// used to identify head of ball linked list
// ball linked list traversals use the head pointer
static ball_t *ball_head;

// used to identify tail of ball linked list
// thread_create_ball uses the tail pointer to add balls
// only used in one thread, so no need for semaphore
static ball_t *ball_tail;

// counter to store current number of balls in ball LL
static uint32_t ball_count;

//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////

// game arena section
const static Rectangle section_ballsy_game_arena[] =
{
     {BALLSY_GAME_ARENA_X_MIN, BALLSY_GAME_ARENA_X_MAX,
      BALLSY_GAME_ARENA_Y_MIN, BALLSY_GAME_ARENA_Y_MAX,
      BALLSY_GAME_ARENA_COLOR}
};

/************************************************************************************
 * Name: add_ball
 * Purpose: Helper function to add a ball to the static balls[] array "linked list".
 *          This function is only called when it is guaranteed that a ball is
 *          available to be created (ball_count < 20).
 *          The ball is added to the end of the ball "linked list", using the
 *          ball_tail pointer. A pointer to the ball structure created is returned
 *          from this helper function.
 * Input(s): N/A
 * Output: ball_t *ball
 ***********************************************************************************/
static inline ball_t* add_ball(void)
{
    // counter used to determine which ball is available in ball list (used for list traversal)
    static uint32_t temp_ball_counter;

    // wait for the ball count to be available
    G8RTOS_semaphore_wait(&semaphore_ball_count);

    // if no elements yet exist in ball list
    if (ball_count == 0)
    {
        ball_head = &balls[0];     // used to identify head in ball linked list
        ball_tail = &balls[0];     // used to identify tail in ball linked list
        temp_ball_counter = 0;
        balls[0].next = &balls[0];
        balls[0].prev = &balls[0];
        ball_head->next = &balls[0];        // IS THIS REDUNDANT?
        ball_tail->next = &balls[0];
        ball_head->prev = &balls[0];
        ball_tail->prev = &balls[0];
    }
    else    // if other elements exist in ball list
    {
        // find first "dead" (available) ball to be instantiated
        // arbitrarily start list traversal from beginning of ball list
        // there is guaranteed to be a "dead" ball
        for (temp_ball_counter = 0; temp_ball_counter < MAX_BALLS; temp_ball_counter++)
        {
            if (balls[temp_ball_counter].isAlive == false)
            {
                break;      // first "dead" ball found, retain temp_ball_counter value
            }
        }

        // add the new ball at the end of the ball list (also before the head)
        // update necessary ball linked list pointers
        balls[temp_ball_counter].prev = ball_tail;
        balls[temp_ball_counter].next = ball_head;
        ball_head->prev = &balls[temp_ball_counter];
        ball_tail->next = &balls[temp_ball_counter];
        ball_tail = &balls[temp_ball_counter];      // update tail of ball list
    }

    // activate ball (make ball "alive")
    balls[temp_ball_counter].isAlive = true;


    // wait for the global coordinates variable to be available
    G8RTOS_semaphore_wait(&semaphore_coordinates);

    // check for x-axis touch coordinates being at edge of screen
    // if so, fit whole 4x4 ball on screen
    if (coordinates.x < 2)
    {
        balls[temp_ball_counter].x_coordinate = 2;
    }
    else if (coordinates.x > 318)
    {
        balls[temp_ball_counter].x_coordinate = 318;
    }
    else
    {
        balls[temp_ball_counter].x_coordinate = coordinates.x;
    }

    // check for y-axis touch coordinates being at edge of screen
    // if so, fit whole 4x4 ball on screen
    if (coordinates.y < 2)
    {
        balls[temp_ball_counter].y_coordinate = 2;
    }
    else if (coordinates.y > 238)
    {
        balls[temp_ball_counter].y_coordinate = 238;
    }
    else
    {
        balls[temp_ball_counter].y_coordinate = coordinates.y;
    }

    // signal that the global coordinates variable is available
    G8RTOS_semaphore_signal(&semaphore_coordinates);

    // initialize ball's velocity to zero (will be updated in ball's thread)
    balls[temp_ball_counter].x_velocity = 0;
    balls[temp_ball_counter].y_velocity = 0;

    // initialize the ball's thread with the respective thread's ID
    balls[temp_ball_counter].thread_id = G8RTOS_get_tid();

    // set a sorta-random color (uint16_t) to the newly created ball
    // rand() provides random integer between 0 and RAND_MAX (32767)
    // maximize RGB color range by multiplying rand() range by 2 and by adding 1
    balls[temp_ball_counter].color = ((rand() * 2) + 1);

    // update the global ball count
    ball_count = ball_count + 1;

    // signal that the ball count is available
    G8RTOS_semaphore_signal(&semaphore_ball_count);

    return &balls[temp_ball_counter];     // return address of created ball
}

/************************************************************************************
 * Name: remove_ball
 * Purpose: Thread to remove a ball from the LCD.
 * Input(s): ball_t *ball
 * Output: N/A
 ***********************************************************************************/
static inline void remove_ball(ball_t *ball)
{
    // static variables to hold criteria for drawing the necessary ball rectangles
    // if ball does not need to be wrapped, the second draw rectangle function call
    // will not carry out!
    static int16_t xStart1, xStart2;
    static int16_t xEnd1, xEnd2;
    static int16_t yStart1, yStart2;
    static int16_t yEnd1, yEnd2;

    // update static variables with the respective ball coordinates (declared above)
    xStart1 = (ball->x_coordinate)-2;
    xEnd1 = (ball->x_coordinate)+2;
    yStart1 = (ball->y_coordinate)-2;
    yEnd1 = (ball->y_coordinate)+2;

    // initialize ball coordinates for second rectangle every time
    // initially assuming ball is not wrapped!
    xStart2 = 0;
    xEnd2 = 0;
    yStart2 = 0;
    yEnd2 = 0;

    // alter static coordinate variables, update rectangle coordinates,
    // only if ball needs to be wrapped
    // second rectangle variables must be set first,
    // since they rely off of original values of first rectangle variables
    if (xStart1 < BALLSY_GAME_ARENA_X_MIN)
    {
        // second rectangle to be drawn for ball
        xStart2 = BALLSY_GAME_ARENA_X_MAX + xStart1;       // recall that xStart1 < 0 in this case
        xEnd2 = BALLSY_GAME_ARENA_X_MAX;

        // first rectangle to be drawn for ball
        xStart1 = BALLSY_GAME_ARENA_X_MIN;
        xEnd1 = xEnd1 - BALLSY_GAME_ARENA_X_MIN;

        // update second rectangle's y-axis variables, only if they haven't
        // already been changed
        if ((yStart2 == 0) && (yEnd2 == 0))
        {
            yStart2 = yStart1;
            yEnd2 = yEnd1;
        }

    }
    if (xEnd1 > BALLSY_GAME_ARENA_X_MAX)
    {
        // second rectangle to be drawn for ball
        xStart2 = BALLSY_GAME_ARENA_X_MIN;
        xEnd2 = xEnd1 - BALLSY_GAME_ARENA_X_MAX;

        // first rectangle to be drawn for ball
        xStart1 = xStart1;      // shown just for completeness
        xEnd1 = BALLSY_GAME_ARENA_Y_MAX;

        // update second rectangle's y-axis variables, only if they haven't
        // already been changed
        if ((yStart2 == 0) && (yEnd2 == 0))
        {
            yStart2 = yStart1;
            yEnd2 = yEnd1;
        }
    }
    if (yStart1 < BALLSY_GAME_ARENA_Y_MIN)
    {
        // second rectangle to be drawn for ball
        yStart2 = BALLSY_GAME_ARENA_Y_MAX + yStart1;       // recall that yStart1 < 0 in this case
        yEnd2 = BALLSY_GAME_ARENA_Y_MAX;

        // first rectangle to be drawn for ball
        yStart1 = BALLSY_GAME_ARENA_Y_MIN;
        yEnd1 = yEnd1 - BALLSY_GAME_ARENA_Y_MIN;

        // update second rectangle's x-axis variables, only if they haven't
        // already been changed
        if ((xStart2 == 0) && (xEnd2 == 0))
        {
            xStart2 = xStart1;
            xEnd2 = xEnd1;
        }
    }
    if (yEnd1 > BALLSY_GAME_ARENA_Y_MAX)
    {
        // second rectangle to be drawn for ball
        yStart2 = BALLSY_GAME_ARENA_Y_MIN;
        yEnd2 = yEnd1 - BALLSY_GAME_ARENA_Y_MAX;

        // first rectangle to be drawn for ball
        yStart1 = yStart1;      // shown just for completeness
        yEnd1 = BALLSY_GAME_ARENA_Y_MAX;

        // update second rectangle's x-axis variables, only if they haven't
        // already been changed
        if ((xStart2 == 0) && (xEnd2 == 0))
        {
            xStart2 = xStart1;
            xEnd2 = xEnd1;
        }
    }

    // if ball is to be wrapped on screen from updated x and y coordinates,
    // draw two rectangles accordingly
    // if ball is not to be wrapped, second draw rectangle function will not carry out
    LCD_DrawRectangle(xStart1, xEnd1, yStart1, yEnd1, LCD_BLACK);
    LCD_DrawRectangle(xStart2, xEnd2, yStart2, yEnd2, LCD_BLACK);

    return;
}

/************************************************************************************
 * Name: display_ball
 * Purpose: Thread to display a ball on the LCD.
 * Input(s): ball_t *ball
 * Output: N/A
 ***********************************************************************************/
static inline void display_ball(ball_t *ball)
{
    // static variables to hold criteria for drawing the necessary ball rectangles
    // if ball does not need to be wrapped, the second draw rectangle function call
    // will not carry out!
    static int16_t xStart1, xStart2;
    static int16_t xEnd1, xEnd2;
    static int16_t yStart1, yStart2;
    static int16_t yEnd1, yEnd2;

    // update static variables with the respective ball coordinates (declared above)
    xStart1 = (ball->x_coordinate)-2;
    xEnd1 = (ball->x_coordinate)+2;
    yStart1 = (ball->y_coordinate)-2;
    yEnd1 = (ball->y_coordinate)+2;

    // initialize ball coordinates for second rectangle every time
    // initially assuming ball is not wrapped!
    xStart2 = 0;
    xEnd2 = 0;
    yStart2 = 0;
    yEnd2 = 0;

    // alter static coordinate variables, update rectangle coordinates,
    // only if ball needs to be wrapped
    // second rectangle variables must be set first,
    // since they rely off of original values of first rectangle variables
    if (xStart1 < BALLSY_GAME_ARENA_X_MIN)
    {
        // second rectangle to be drawn for ball
        xStart2 = BALLSY_GAME_ARENA_X_MAX + xStart1;       // recall that xStart1 < 0 in this case
        xEnd2 = BALLSY_GAME_ARENA_X_MAX;

        // first rectangle to be drawn for ball
        xStart1 = BALLSY_GAME_ARENA_X_MIN;
        xEnd1 = xEnd1 - BALLSY_GAME_ARENA_X_MIN;

        // update second rectangle's y-axis variables, only if they haven't
        // already been changed
        if ((yStart2 == 0) && (yEnd2 == 0))
        {
            yStart2 = yStart1;
            yEnd2 = yEnd1;
        }

    }
    if (xEnd1 > BALLSY_GAME_ARENA_X_MAX)
    {
        // second rectangle to be drawn for ball
        xStart2 = BALLSY_GAME_ARENA_X_MIN;
        xEnd2 = xEnd1 - BALLSY_GAME_ARENA_X_MAX;

        // first rectangle to be drawn for ball
        xStart1 = xStart1;      // shown just for completeness
        xEnd1 = BALLSY_GAME_ARENA_X_MAX;

        // update second rectangle's y-axis variables, only if they haven't
        // already been changed
        if ((yStart2 == 0) && (yEnd2 == 0))
        {
            yStart2 = yStart1;
            yEnd2 = yEnd1;
        }
    }
    if (yStart1 < BALLSY_GAME_ARENA_Y_MIN)
    {
        // second rectangle to be drawn for ball
        yStart2 = BALLSY_GAME_ARENA_Y_MAX + yStart1;       // recall that yStart1 < 0 in this case
        yEnd2 = BALLSY_GAME_ARENA_Y_MAX;

        // first rectangle to be drawn for ball
        yStart1 = BALLSY_GAME_ARENA_Y_MIN;
        yEnd1 = yEnd1 - BALLSY_GAME_ARENA_Y_MIN;

        // update second rectangle's x-axis variables, only if they haven't
        // already been changed
        if ((xStart2 == 0) && (xEnd2 == 0))
        {
            xStart2 = xStart1;
            xEnd2 = xEnd1;
        }
    }
    if (yEnd1 > BALLSY_GAME_ARENA_Y_MAX)
    {
        // second rectangle to be drawn for ball
        yStart2 = BALLSY_GAME_ARENA_Y_MIN;
        yEnd2 = yEnd1 - BALLSY_GAME_ARENA_Y_MAX;

        // first rectangle to be drawn for ball
        yStart1 = yStart1;      // shown just for completeness
        yEnd1 = BALLSY_GAME_ARENA_Y_MAX;

        // update second rectangle's x-axis variables, only if they haven't
        // already been changed
        if ((xStart2 == 0) && (xEnd2 == 0))
        {
            xStart2 = xStart1;
            xEnd2 = xEnd1;
        }
    }

    // if ball is to be wrapped on screen from updated x and y coordinates,
    // draw two rectangles accordingly
    // if ball is not to be wrapped, second draw rectangle function will not carry out
    LCD_DrawRectangle(xStart1, xEnd1, yStart1, yEnd1, (ball->color));
    LCD_DrawRectangle(xStart2, xEnd2, yStart2, yEnd2, (ball->color));


    return;
}

/************************************************************************************
 * Name: kill_ball
 * Purpose: Thread to potentially kill a ball in the static ball array "linked list".
 *          If a ball is meant to be killed, the respective ball thread in the TCB
 *          array "linked list" is killed.
 *          This function returns a boolean value, as to determine whether or not
 *          a ball was actually killed. A return value of "true" will indicate that
 *          a ball was killed, and a return value of "false" will indicate that a
 *          ball was not killed.
 * Input(s): N/A
 * Output: bool (ballWasKilled)
 ***********************************************************************************/
static inline bool kill_ball(void)
{
    // static temp ball structure to traverse active ball list
    static ball_t *temp_ball;

    // local boolean to determine whether ball was killed
    static bool ballKilled;

    // check if no balls exist
    if (ball_count == 0)
    {
        ballKilled = false;
    }
    else
    {

        // otherwise, at least one ball exists,
        // determine whether or not it should be killed
        // initialize ballKilled to false for logical purposes
        ballKilled = false;

        /*
        // wait for coordinates FIFO to be available
        G8RTOS_semaphore_wait(&semaphore_coordinates);

        // read the coordinates FIFO and set the global coordinates variable
        // the coordinates were determined upon the LCD Touch Panel being pressed,
        // in the thread_debounce_LCD thread
        coordinates.x = G8RTOS_fifo_read(FIFO_coordinates);
        coordinates.y = G8RTOS_fifo_read(FIFO_coordinates);

        // signal that the coordinates FIFO is available
        G8RTOS_semaphore_signal(&semaphore_coordinates);
        */

        // wait for the ball list to be available
        G8RTOS_semaphore_wait(&semaphore_ball_list);

        // wait for the ball LL head pointer to be available
        G8RTOS_semaphore_wait(&semaphore_ball_head);

        // traverse through all alive balls (the balls static linked list),
        // as to see if a ball is meant to be destroyed (user touched ball on screen)
        // start list traversal at head of ball list
        temp_ball = ball_head;

        // signal that ball LL head pointer is available
        G8RTOS_semaphore_signal(&semaphore_ball_head);

        // wait for the global coordinates variable to be available
        G8RTOS_semaphore_wait(&semaphore_coordinates);

        for (uint8_t i = 0; i < ball_count; i++)
        {

            // check if user press was within 15 units of active ball
            // this larger than 2 value accounts for inaccuracies in TP ADC readings
            if ( ((temp_ball->x_coordinate - coordinates.x) < 40 &&
            (temp_ball->x_coordinate - coordinates.x) >= -40) &&
            ((temp_ball->y_coordinate - coordinates.y) < 40 &&
            (temp_ball->y_coordinate - coordinates.y) >= -40) )
            {
                temp_ball->isAlive = false;   // ball was found, kill it

                // if killing the only ball
                if (ball_count == 1)
                {
                    ball_head = NULL;       // reset head to NULL
                    ball_tail = NULL;       // reset tail to NULL
                }
                // killing one of at least two balls
                else
                {
                    // check if killed ball was either head or tail of linked list
                    if (temp_ball == ball_head)
                    {
                        ball_head = temp_ball->next;        // update head of list
                    }
                    if (temp_ball == ball_tail)
                    {
                        ball_tail = temp_ball->prev;        // update tail of list
                    }

                    // update necessary pointers in the ball linked list
                    temp_ball->prev->next = temp_ball->next;
                    temp_ball->next->prev = temp_ball->prev;

                }

                // delete ball from screen
                remove_ball(temp_ball);

                // update the global ball count
                ball_count = ball_count - 1;

                // kill respective ball thread
                G8RTOS_kill_thread(temp_ball->thread_id);

                ballKilled = true;        // ball was killed

                // do not kill any more ball threads
                break;
            }

            // point temp_ball to the next ball in the active ball LL
            temp_ball = temp_ball->next;
        }

        // signal that the global coordinates variable is available
        G8RTOS_semaphore_signal(&semaphore_coordinates);

        // signal that the ball list is available
        G8RTOS_semaphore_signal(&semaphore_ball_list);

    }


    return ballKilled;      // return whether or not a ball was killed
}

/************************************************************************************
 * Name: update_ball_velocity
 * Purpose: Helper function to update a ball's velocity.
 * Input(s): ball_t *ball
 * Output: N/A
 ***********************************************************************************/
static inline void update_ball_velocity(ball_t *ball)
{
    static int16_t data_accel_x;        // x-axis accelerometer data (local to the thread)
    static int16_t data_accel_y;        // y-axis accelerometer data (local to the thread)

    G8RTOS_semaphore_wait(&semaphore_CC3100);
    // exclusive access to sensor (no need for a semaphore)
    bmi160_read_accel_x(&data_accel_x);     // read x-axis accelerometer data
    bmi160_read_accel_y(&data_accel_y);     // read y-axis accelerometer data

    G8RTOS_semaphore_signal(&semaphore_CC3100);

    // determine ball's x-axis velocity
    if (data_accel_x > 15000)
        ball->x_velocity = 5;
    else if (data_accel_x > 12000)
        ball->x_velocity = 4;
    else if (data_accel_x > 9000)
        ball->x_velocity = 3;
    else if (data_accel_x > 6000)
        ball->x_velocity = 2;
    else if (data_accel_x > 3000)
        ball->x_velocity = 1;
    else if (data_accel_x > 0000)
        ball->x_velocity = 0;
    else if (data_accel_x >= -3000)
        ball->x_velocity = 0;
    else if (data_accel_x > -6000)
        ball->x_velocity = -1;
    else if (data_accel_x > -9000)
        ball->x_velocity = -2;
    else if (data_accel_x > -12000)
        ball->x_velocity = -3;
    else if (data_accel_x > -15000)
        ball->x_velocity = -4;
    else        // data_accel_x < -15000
        ball->x_velocity = -5;

    // determine ball's y-axis velocity
    if (data_accel_y > 15000)
        ball->y_velocity = -5;
    else if (data_accel_y > 12000)
        ball->y_velocity = -4;
    else if (data_accel_y > 9000)
        ball->y_velocity = -3;
    else if (data_accel_y > 6000)
        ball->y_velocity = -2;
    else if (data_accel_y > 3000)
        ball->y_velocity = -1;
    else if (data_accel_y > 0000)
        ball->y_velocity = 0;
    else if (data_accel_y >= -3000)
        ball->y_velocity = 0;
    else if (data_accel_y > -6000)
        ball->y_velocity = 1;
    else if (data_accel_y > -9000)
        ball->y_velocity = 2;
    else if (data_accel_y > -12000)
        ball->y_velocity = 3;
    else if (data_accel_y > -15000)
        ball->y_velocity = 4;
    else        // data_accel_y < -15000
        ball->y_velocity = 5;

    return;
}

/************************************************************************************
 * Name: update_ball_coordinates
 * Purpose: Helper function to update a ball's coordinates.
 * Input(s): ball_t *ball
 * Output: N/A
 ***********************************************************************************/
static inline void update_ball_coordinates(ball_t *ball)
{
    // calculate x-axis displacement from current position,
    // based on calculated velocity (see update_ball helper function)
    switch (ball->x_velocity)
    {
        case -5:
            ball->x_coordinate = ball->x_coordinate - 32;
            break;
        case -4:
            ball->x_coordinate = ball->x_coordinate - 16;
            break;
        case -3:
            ball->x_coordinate = ball->x_coordinate - 8;
            break;
        case -2:
            ball->x_coordinate = ball->x_coordinate - 4;
            break;
        case -1:
            ball->x_coordinate = ball->x_coordinate - 2;
            break;
        case 0:
            ball->x_coordinate = ball->x_coordinate;        // included for completeness
            break;
        case 1:
            ball->x_coordinate = ball->x_coordinate + 2;
            break;
        case 2:
            ball->x_coordinate = ball->x_coordinate + 4;
            break;
        case 3:
            ball->x_coordinate = ball->x_coordinate + 8;
            break;
        case 4:
            ball->x_coordinate = ball->x_coordinate + 16;
            break;
        case 5:
            ball->x_coordinate = ball->x_coordinate + 32;
            break;
        default:
            break;
    }

    // calculate y-axis displacement from current position,
    // based on calculated velocity (see update_ball helper function)
    switch (ball->y_velocity)
    {
        case -5:
            ball->y_coordinate = ball->y_coordinate - 32;
            break;
        case -4:
            ball->y_coordinate = ball->y_coordinate - 16;
            break;
        case -3:
            ball->y_coordinate = ball->y_coordinate - 8;
            break;
        case -2:
            ball->y_coordinate = ball->y_coordinate - 4;
            break;
        case -1:
            ball->y_coordinate = ball->y_coordinate - 2;
            break;
        case 0:
            ball->y_coordinate = ball->y_coordinate;        // included for completeness
            break;
        case 1:
            ball->y_coordinate = ball->y_coordinate + 2;
            break;
        case 2:
            ball->y_coordinate = ball->y_coordinate + 4;
            break;
        case 3:
            ball->y_coordinate = ball->y_coordinate + 8;
            break;
        case 4:
            ball->y_coordinate = ball->y_coordinate + 16;
            break;
        case 5:
            ball->y_coordinate = ball->y_coordinate + 32;
            break;
        default:
            break;
    }

    // check if ball should completely wrap around screen
    if (ball->x_coordinate <= BALLSY_GAME_ARENA_X_MIN - 3)
    {
        ball->x_coordinate = BALLSY_GAME_ARENA_X_MAX - 2;
    }
    else if (ball->x_coordinate >= BALLSY_GAME_ARENA_X_MAX + 3)
    {
        ball->x_coordinate = BALLSY_GAME_ARENA_X_MIN + 2;
    }
    if (ball->y_coordinate <= BALLSY_GAME_ARENA_Y_MIN - 3)
    {
        ball->y_coordinate = BALLSY_GAME_ARENA_Y_MAX - 2;
    }
    else if (ball->y_coordinate >= BALLSY_GAME_ARENA_Y_MAX + 3)
    {
        ball->y_coordinate = BALLSY_GAME_ARENA_Y_MIN + 2;
    }

    // otherwise, ball will either already be completely on screen,
    // or ball will be currently wrapping around screen (two rectangles)
    // the display_ball helper function will handle the drawing of the rectangle(s)

    return;
}
/************************************************************************************
 * Name: update_ball
 * Purpose: Helper function to update a ball's position on the LCD.
 * Input(s): ball_t *ball
 * Output: N/A
 ***********************************************************************************/
static inline void update_ball(ball_t *ball)
{
    // if ball is currently wrapped on the screen,
    // delete the two rectangles accordingly

    // otherwise, delete the single respective rectangle
    remove_ball(ball);

    // read accelerometer and update velocity accordingly
    update_ball_velocity(ball);

    // update x and y coordinates of ball accordingly,
    // based on calculated velocity
    update_ball_coordinates(ball);

    // otherwise, display the ball accordingly (wrapped or not wrapped)
    display_ball(ball);

    return;
}
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////


////////////////////////////////PUBLIC FUNCTIONS/////////////////////////////////////

/************************************************************************************
 * Name: aperiodic_port4
 * Purpose: Aperiodic event to register up push-button and right push-button
 *          presses on daughter board. The LCD TouchPanel is also handled in this
 *          aperiodic thread.
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_port4(void)
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
        // disable pin 0 interrupt (will be rearmed after proper debounce time in thread_create_ball)
        GPIO_disableInterrupt(GPIO_PORT_P4, GPIO_PIN0);

        // create LCD Touch Panel debouncing thread (highest priority)
        G8RTOS_add_thread(thread_debounce_LCD, 50, "debounce_LCD");      // AM I HANDLING THE NAME CORRECTLY!??
    }
//    // if up push-button triggered interrupt
//    else if (status == 10)       // 10 = 2*(4+1)
//    {
//        counter0++;
//        leds_update(RED, counter0);
//        leds_update(GREEN, 0x0000);
//        leds_update(BLUE, 0x0000);
//    }
//    // if right push-button triggered interrupt
//    else if (status == 12)       // 12 = 2*(5+1)
//    {
//        counter1++;
//        leds_update(GREEN, counter1);
//        leds_update(RED, 0x0000);
//        leds_update(BLUE, 0x0000);
//    }

}

/************************************************************************************
 * Name: aperiodic_port5
 * Purpose: Aperiodic event to register left push-button and down push-button
 *          presses on daughter board.
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_port5(void)
{
    uint8_t status;     // used to identify which P5 pin triggered the ISR (pins 4 or 5)

    status = P5IV;      // P4IV = (2*(n+1)) where n is the pin number of the
                        // lowest bit with a pending interrupt.
                        // this access will only clear flag n.


    // if triggering switch is high after debounce time
    if ( ( P5->IN & ( 1 << (status/2) - 1 ) ) == ( 1 << (status/2) - 1 ) )
    {
        return;     // ISR improperly called
    }

    // otherwise, interrupt was properly called

    // if down push-button triggered interrupt
    if (status == 10)       // 10 = 2*(4+1)
    {
        counter2++;
        leds_update(BLUE, counter2);
        leds_update(GREEN, 0x0000);
        leds_update(RED, 0x0000);
    }
    // if left push-button triggered interrupt
    if (status == 12)       // 12 = 2*(5+1)
    {
        counter3++;
        leds_update(RED, counter3);
        leds_update(GREEN, counter3);
        leds_update(BLUE, counter3);
    }

}

/*
/************************************************************************************
 * Name: pet_joystick
 * Purpose: Read joystick's x-axis data, and then write this data to the joystick
 *          FIFO. Additionally, this PET toggles P9, pin 6 on the MSP432.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************
void pet_joystick(void)
{
    // variables to store coordinates for x and y joystick axes
    static int16_t joystick_coord_x;
    static int16_t joystick_coord_y;

    // read x and y coordinates from daughter board joystick
    GetJoystickCoordinates(&joystick_coord_x, &joystick_coord_y);

    // remove offset from joystick coordinate axes
    // values depend on individual joystick
    joystick_coord_x += 225;
    joystick_coord_y -= 17;

    // write "actual" data to joystick FIFO
    // it is necessary to cast joystick data to int32_t*
    G8RTOS_fifo_write(fifo_joystick, joystick_coord_x);

    // toggle P9, pin 6
    // use bit-banding to avoid affecting other pins on P9
    BITBAND_PERI(P9->OUT, 6) ^= 1;

    return;
}


/************************************************************************************
 * Name: pet_uart
 * Purpose: If global boolean light variable is true, print out temperature
 *          (in fahrenheit) and decayed average for the x-coordinate of the joystick.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************
void pet_uart(void)
{

    if (writeUART)      // if RMS of light data < 5000
    {
        BackChannelPrintIntVariable("Temperature (in Fahrenheit)", temperature_data);
        BackChannelPrintIntVariable("Decayed Average for X-Coordinate of Joystick:",
                                    joystick_data_x_avg_decayed);
    }

    return;
}
*/

/************************************************************************************
 * Name: pet_test1
 * Purpose: used to test dynamic PETS
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void pet_test1(void)
{
    return;
}

/************************************************************************************
 * Name: thread_ball
 * Purpose: High priority thread created from touch being made to LCD Touch Panel.
 *          This thread creates a new thread_create_ball thread, which will create
 *          and display a 4x4 ball of a random RGB color
 *          on the LCD display screen, based on the location of the user's press.
 *          This thread will then update the ball's location on the LCD screen every
 *          30 milliseconds (~30 frames per second), based on the MSP432's
 *          accelerometer, and then sleep.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_ball(void)
{
    // create static ball for this thread's instance
    // ball pointer used to update coordinates
    ball_t *ball;

    // wait for ball list to be available
    G8RTOS_semaphore_wait(&semaphore_ball_list);

    ball = add_ball();       // create a ball

    // signal that ball list is available
    G8RTOS_semaphore_signal(&semaphore_ball_list);

    // wait for LCD to be available
    G8RTOS_semaphore_wait(&semaphore_LCD);

    /*
    // wait for ball list to be available
    G8RTOS_semaphore_wait(&semaphore_ball_list);

    // draw initial placement of ball (test code)
    LCD_DrawRectangle( (ball->x_coordinate - 2), (ball->x_coordinate + 2),
                       (ball->y_coordinate - 2), (ball->y_coordinate + 2),
                        ball->color);

    // signal that ball list is available
    G8RTOS_semaphore_signal(&semaphore_ball_list);
    */

    // signal that LCD is available for use
    G8RTOS_semaphore_signal(&semaphore_LCD);

    // for the remainder of this thread's life,
    // update ball location roughly every 30ms
    while(1)
    {
        // wait for ball list to be available
        G8RTOS_semaphore_wait(&semaphore_ball_list);

        // wait for LCD to be available
        G8RTOS_semaphore_wait(&semaphore_LCD);

        // update ball on LCD screen (using static helper functions)
        update_ball(ball);

        // signal that the LCD is available
        G8RTOS_semaphore_signal(&semaphore_LCD);

        // signal that ball list is available
        G8RTOS_semaphore_signal(&semaphore_ball_list);

        // create a ~30fps refresh rate
        G8RTOS_thread_sleep(30);

    }

}

/************************************************************************************
 * Name: thread_ball_program_init
 * Purpose: High priority thread to initialize the balls static array "linked list"
 *          (initialize all balls to being "dead"), to initialize the ball counter,
 *          and the respective head and tail pointers of the ball "linked list".
 *          After conducting the appropriate initializations, the thread kills itself.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_ball_program_init(void)
{
    // initialize ball program semaphores
    G8RTOS_semaphore_init(&semaphore_ball_list, 1);
    G8RTOS_semaphore_init(&semaphore_coordinates, 1);
    G8RTOS_semaphore_init(&semaphore_LCD, 1);
    G8RTOS_semaphore_init(&semaphore_ball_count, 1);
    G8RTOS_semaphore_init(&semaphore_ball_head, 1);

    /* redraw text arena */
    LCD_DrawSection(section_ballsy_game_arena,
                    (sizeof(section_ballsy_game_arena)/sizeof(section_ballsy_game_arena[0])));

    // semaphores are not needed when initializing data members in this function
    // this function is guaranteed to be called before any other conflicting code

    // initialize FIFO counter (defined in G8RTOS_ipc.h)
    FIFO_count = 0;

    // initialize ball program FIFOs
    // FIFO indices are defined in threads.h
    G8RTOS_add_fifo(FIFO_coordinates);

    // initialize static balls array ("linked list")
    for (uint8_t i = 0; i < MAX_BALLS; i++)
    {
        balls[i].isAlive = false;        // kill ball in balls[] (make available)
    }

    // initialize the ball counter
    ball_count = 0;

    // initialize the ball LL head and tail pointers to NULL
    // NULL is defined in G8RTOS_structures.h
    ball_head = NULL;
    ball_tail = NULL;

    // re-enable P4 interrupts to allow touches to be made to LCD TP
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN0);

    /* add necessary aperiodic thread */
    G8RTOS_add_aperiodic_thread(aperiodic_port4, PORT4_IRQn, 6);       // add PORT4 interrupts

    // kill this thread
    G8RTOS_kill_current_thread();

}

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
void thread_debounce_LCD(void)
{
    // static Point variable used to read LCD Touch Panel
    //static Point temp_coordinates;

    // wait for coordinates variable to be available
    G8RTOS_semaphore_wait(&semaphore_coordinates);

    // read x and y coordinates from LCD Touch Panel
    coordinates = TP_ReadXY();

    // signal that coordinates variable is available
    G8RTOS_semaphore_signal(&semaphore_coordinates);

    // these coordinates are used to initialize new ball
    // store coordinates in the coordinates FIFO
    // store the x-coordinate before the y-coordinate
    //G8RTOS_fifo_write(FIFO_coordinates, temp_coordinates.x);
    //G8RTOS_fifo_write(FIFO_coordinates, temp_coordinates.y);

    // if touch was within game screen bounds
    if (coordinates.x > MUPHONE_HEADER_BAR_DIVIDER_Y_MAX)
    {
        // wait for ball count to be available
        G8RTOS_semaphore_wait(&semaphore_ball_count);

        // wait for LCD to be available
        G8RTOS_semaphore_wait(&semaphore_LCD);

        // determine whether or not ball is meant to be killed (user touched ball)
        // if ball is not killed, create a new ball thread (if space is available)
        if ( (!kill_ball()) && (ball_count < 20) )     // kill_ball is statically defined above
        {
            // create another ball thread
            G8RTOS_add_thread(thread_ball, 60, "ball");
        }

        // signal that the LCD is available
        G8RTOS_semaphore_signal(&semaphore_LCD);

        // signal that ball count is available
        G8RTOS_semaphore_signal(&semaphore_ball_count);
    }

    // debounce LCD screen for 500ms
    G8RTOS_thread_sleep(500);

    // re-enable LCD Touch Panel interrupt
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN0);

    // clear interrupt flag
    GPIO_clearInterruptFlag(GPIO_PORT_P4, GPIO_PIN0);

    // kill thread, as to allow other balls to be created
    G8RTOS_kill_current_thread();
}

/************************************************************************************
 * Name: thread_idle
 * Purpose: To prevent infinite loops for situations where there are only sleeping/
 *          blocked threads. This thread will never be asleep or blocked.
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************/
void thread_idle(void)
{
    while (1);      // thread is always active
}


/*
/************************************************************************************
 * Name: thread_joystick_output
 * Purpose: Read joystick FIFO, calculate decayed average (see Lab 3 document),
 *          and output data to green LEDs as described in Lab 3 document
 * Input(s): N/A
 * Output: N/A
 ************************************************************************************
void thread_lcd(void)
{
    static uint16_t led_value;     // value used to write to LEDs

    while (1)
    {
        // calculate the 50% decayed average as described in Lab 3
        joystick_data_x_avg_decayed = (joystick_data_x_avg_decayed +
                                      G8RTOS_fifo_read(fifo_joystick)) >> 1;

        if (joystick_data_x_avg_decayed > 6200)
            led_value = 0xF000;
        else if (joystick_data_x_avg_decayed > 4200)
            led_value = 0x7000;
        else if (joystick_data_x_avg_decayed > 2200)
            led_value = 0x3000;
        else if (joystick_data_x_avg_decayed > 700)
            led_value = 0x1000;
        else if (joystick_data_x_avg_decayed > -500)
            led_value = 0x0000;
        else if (joystick_data_x_avg_decayed > -1800)
            led_value = 0x0800;
        else if (joystick_data_x_avg_decayed > -3800)
            led_value = 0x0C00;
        else if (joystick_data_x_avg_decayed > -5800)
            led_value = 0x0E00;
        else        // joystick_data_x_avg_decayed > -8000
            led_value = 0x0F00;


        G8RTOS_semaphore_wait(&semaphore_led);        // wait until LEDs are available for use

        // exclusive access to LEDs

        // update LEDs with calculated LED value
        leds_update(GREEN, led_value);

        G8RTOS_semaphore_signal(&semaphore_led);      // signal that LEDs are available for use
    }
}
*/


/////////////////////////////END OF PUBLIC FUNCTIONS/////////////////////////////////


/////////////////////////////////PRIVATE FUNCTIONS///////////////////////////////////
//////////////////////////////END OF PRIVATE FUNCTIONS///////////////////////////////


////////////////////////////INTERRUPT SERVICE ROUTINES///////////////////////////////
////////////////////////END OF INTERRUPT SERVICE ROUTINES////////////////////////////

//////////////////////////////////END OF threads.c///////////////////////////////////
