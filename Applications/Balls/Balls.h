/*
 * Threads.h
 *
 *  Created on: Sep 12, 2017
 *      Author: Brit Chesley
 */
#include "G8RTOS.h"
#include "BSP.h"

#ifndef BALLS_H
#define BALLS_H



#define MAX_NUMBER_BALLS 20
#define BACKGROUND_COLOR LCD_BLACK


typedef struct ball{

    Point position;
    bool dead; //ball is initially dead
    int32_t velocity;
    int32_t accelerationX;
    int32_t accelerationY;
    int16_t color; //color of ball
    int32_t threadID;


    semaphore_t memberVarSem;

}ball_t;


ball_t ballArray[MAX_NUMBER_BALLS];



//initialize balls application
void thread_init_balls_app();

/*Thread declarations*/

//background thread
/*Thread: readAccel
 * type: background
 * purpose: this thread reads accelerometer data, then sleeps
 *
 */
void ReadAccel(void);

//Aperiodic
/* Thread: LCDTap
 * Type: Aperiodic
 * purpose: this aperiodic thread is triggered whenever a tap on the LCD is recognized
 *
 */
void LCDtap(void);

//background
/*Thread: WaitForTap
 * type: background
 * purpose: this thread is created when a tap is detected on the TP.
 *          This debounces the TP and either creates or destroys a ball thread
 *
 */
void WaitForTap(void);

/*Thread: idle
 * type: background
 * purpose: prevent deadlock
 */
//void Idle(void);

/*Thread: MoveBall
 * Type: Background
 * purpose: this thread moves a ball. This thread is dynamically created based on the TP tap.
 *          There can be up to 20 instances of this thread
 *
 */
void MoveBall(void);



#endif /* THREADS_H_ */
