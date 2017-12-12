/*
 * Threads.c
 *
 *  Created on: Sep 12, 2017
 *      Author: Brit Chesley
 */

#include "G8RTOS.h"
#include "G8RTOS_Semaphores.h"
#include "G8RTOS_Scheduler.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "Balls.h"
/* contains all thread source code and semaphore initializations */

semaphore_t ballArraySem;
semaphore_t indexOfBall;
semaphore_t LCD;
semaphore_t accel;

/********** Global Variables (usually have corresponding semaphores) *****/
uint16_t NumberCurrentBalls=0;

uint32_t ballIndex=0;

/* accelerometer data */
s16 xAccelData = 0;
s16 yAccelData = 0;


uint16_t colorArray[] = {LCD_WHITE, LCD_BLUE, LCD_RED,
                      LCD_CYAN, LCD_YELLOW, LCD_PURPLE,
                     LCD_ORANGE, LCD_PINK, LCD_OLIVE};

/********** enums used for FIFOs ****************/
enum {

    POINT_FIFO = 0, //high byte is x position, low byte is y position

};


void thread_init_balls_app()
{

    /* ADD THREADS */
    G8RTOS_add_thread(ReadAccel, 40, "ReadAccel");


    G8RTOS_add_aperiodic_thread(LCDtap, PORT4_IRQn,6);

    //LCD_Init(1);
    /*** add periodic threads ***/

    G8RTOS_semaphore_init(&ballArraySem, 1); //initialize i2c sensor semaphore to be 1 (ready to be used)
    G8RTOS_semaphore_init(&indexOfBall, 1); //initialize i2c led semaphore with 1 (ready to be used)
    G8RTOS_semaphore_init(&LCD, 1);
    G8RTOS_semaphore_init(&accel, 1);



    /**** CANT USE ENUM HERE SINCE ENUM IS DEFINED IN THREADS.C, SINCE PART OF IMPLEMENTATION *****/
    G8RTOS_add_fifo(0); //initialize FIFO for points



    time_t t; //to create a random number generator
    srand((unsigned) time(&t));

    // set all balls to be dead, randomize colors and randomize velocities
    for(int i = 0;i<MAX_NUMBER_BALLS;i++){

        ballArray[i].dead = true;
        ballArray[i].color = colorArray[rand() % 9];
        ballArray[i].velocity = ((rand() % 4) +3); //will have five different velocities

    }


    G8RTOS_kill_current_thread();
    /* Launch operating system */
    //G8RTOS_Launch();
}



/*function: update ball
 * updates the position of the ball on the LCD
 *
 */
static void updateBall(ball_t *prev, ball_t *current){


    //draw previous with background color
    LCD_DrawRectangle(prev->position.x-1, prev->position.x+2, prev->position.y-1, prev->position.y+2, BACKGROUND_COLOR);

    //draw current
    LCD_DrawRectangle(current->position.x -1, current->position.x +2, current->position.y-1, current->position.y+2, current->color);

}

/*Thread: readAccel
 * type: background
 * purpose: this thread reads accelerometer data, then sleeps
 *
 */
void ReadAccel(void){

    while(1){

       G8RTOS_semaphore_wait(&accel);
    /* read both x and y values */
    //read data from accelerometers x-axis
    //acc_X LSB is register 0x12
    //acc_X MSB 0x13
    bmi160_read_accel_x(&xAccelData);

    bmi160_read_accel_y(&yAccelData);

    G8RTOS_semaphore_signal(&accel);

    G8RTOS_thread_sleep(20); //sleep for 20 ms
    }
}


/* Thread: LCDTap
 * Type: Aperiodic
 * purpose: this aperiodic thread is triggered whenever a tap on the LCD is recognized
 *
 */
void LCDtap(void){

    if(P4->IFG & BIT0){

        NVIC_DisableIRQ(PORT4_IRQn);

        //clear flag
        P4->IFG &= 0b11111110;

        /* add thread to debounce */
        G8RTOS_add_thread(WaitForTap, 30, "WaitForTap");
        P4->IFG &= 0b11111110;

        }

}

//for debouncing
/*Thread: WaitForTap
 * type: background
 * purpose: this thread is created when a tap is detected on the TP.
 *          This debounces the TP and either creates or destroys a ball thread
 *
 */
void WaitForTap(){

    /* read position of touch */
    G8RTOS_semaphore_wait(&LCD);
    Point position;
    position = TP_ReadXY();
    G8RTOS_semaphore_signal(&LCD);

    //OS_Sleep(500); //sleep for debounce
    bool delete=false;


    /* NEED TO CHECK IF THERE IS A BALL NEAR THIS POSITION */
    if(NumberCurrentBalls != 0){


        for(int i = 0 ; i < MAX_NUMBER_BALLS ; i++){

            G8RTOS_semaphore_wait(&ballArraySem);
        //  G8RTOS_WaitSemaphore(&ballArray[i].memberVarSem);
         if(!ballArray[i].dead){


             /* if touch is near a ball, delete the ball */

             G8RTOS_semaphore_wait(&ballArray[i].memberVarSem);
        if((ballArray[i].position.x >= position.x - 25 && ballArray[i].position.x <= position.x +25)){
                if(ballArray[i].position.y >= position.y - 25 && ballArray[i].position.y <= position.y+25){

                    ballArray[i].dead = true; //set ball to dead

                    // need to wait for semaphores that the ball thread is using

                    G8RTOS_semaphore_wait(&indexOfBall);

                    G8RTOS_semaphore_wait(&LCD);

                    /* un-draw the square to be deleted */
                    LCD_DrawRectangle(ballArray[i].position.x-1, ballArray[i].position.x+2, ballArray[i].position.y-1, ballArray[i].position.y+2, BACKGROUND_COLOR);

                    G8RTOS_semaphore_signal(&LCD);

                    ballArray[i].position.x = 0;
                    ballArray[i].position.y = 0; //update position to zero, since dead anyways


                     G8RTOS_kill_thread(ballArray[i].threadID); //kill this thread
                      delete = true; //set flag
                      NumberCurrentBalls--;

                      /* signal semaphores */
                      G8RTOS_semaphore_signal(&ballArray[i].memberVarSem);

                      G8RTOS_semaphore_signal(&indexOfBall);

                      G8RTOS_semaphore_signal(&ballArraySem);

                      break;
                }

                /* signal semaphores */
         }
        G8RTOS_semaphore_signal(&ballArray[i].memberVarSem);

    }
         G8RTOS_semaphore_signal(&ballArraySem);

        }
    }
    /* now check if I need to add a ball */
    if(!delete && NumberCurrentBalls != MAX_NUMBER_BALLS){

    /* If made it to this point of thread, need to create new ball */
    /**** Ball Creation ***/

    G8RTOS_fifo_write(position.x << 16 | position.y , POINT_FIFO); //write position to fifo


    G8RTOS_semaphore_wait(&ballArraySem); //wait for appropriate semaphores
    G8RTOS_semaphore_wait(&indexOfBall);

        /* iterate through all balls in array */
        for(int i = 0; i < MAX_NUMBER_BALLS ; i++){

                if(ballArray[i].dead){ //find first dead ball and make alive

                        ballArray[i].dead = false; //initialize ball in array
                        ballArray[i].accelerationX = xAccelData;
                        ballArray[i].accelerationY = yAccelData;

                        G8RTOS_add_thread(MoveBall, 3, "MoveBall"); //create new thread to control ball

                        NumberCurrentBalls++;

                        G8RTOS_semaphore_init(&ballArray[i].memberVarSem, 1); //initialize semaphore for new ball
                        break; //only add one ball

                }

           /* update ball index, which is used to determine where in the array I just added a ball */
           ballIndex = i+1;
        }


    }
    G8RTOS_semaphore_signal(&indexOfBall); //release semaphores
    G8RTOS_semaphore_signal(&ballArraySem);

        G8RTOS_thread_sleep(500); //debounce sleep
        P4->IFG &= 0b11111110;

       //OS_Sleep(500);

        NVIC_EnableIRQ(PORT4_IRQn);

        delete = false; //reset boolean

        G8RTOS_kill_current_thread();


}
/*Thread: MoveBall
 * Type: Background
 * purpose: this thread moves a ball. This thread is dynamically created based on the TP tap.
 *          There can be up to 20 instances of this thread
 *
 */
void MoveBall(){

    /* wait for corresponding semaphores */
    G8RTOS_semaphore_wait(&indexOfBall);

    G8RTOS_semaphore_wait(&ballArraySem);

    G8RTOS_semaphore_wait(&ballArray[ballIndex].memberVarSem);

    /* update the thread id of the ball */
    ballArray[ballIndex].threadID = G8RTOS_get_tid();

    /* read in position via FIFO */
    int32_t temp = G8RTOS_fifo_read(POINT_FIFO);
    int32_t xPosition = (temp >> 16) & 0xffff;
    int32_t yPosition = (temp & 0xffff);
    int32_t velocity = ballArray[ballIndex].velocity;

    /* ball pointer to access the ball this thread corresponds to */
    ball_t * ballPtr = &ballArray[ballIndex];

    /* signal semaphores */
    G8RTOS_semaphore_signal(&ballArray[ballIndex].memberVarSem);
    G8RTOS_semaphore_signal(&ballArraySem);
    G8RTOS_semaphore_signal(&indexOfBall);

    /* previous ball for the helper function */
    ball_t  prevBall;

   while(1){

       /* wait for semaphores */
       G8RTOS_semaphore_wait(&indexOfBall);
       G8RTOS_semaphore_wait(&ballArraySem);
       G8RTOS_semaphore_wait(&ballPtr->memberVarSem);

       prevBall.position = ballPtr->position;


       G8RTOS_semaphore_wait(&accel);

       /* update position of ball via accelerometer data */
       if(xAccelData > 14000 ){
           xPosition = xPosition +5*velocity;   //move right
       }
       else if(xAccelData > 9000){
            //position.x=
           xPosition = xPosition +4*velocity;
       }
       else if(xAccelData > 7000){
           xPosition = xPosition +3*velocity;
       }
       else if(xAccelData > 5000){

           xPosition = xPosition + 2*velocity;
       }
       else if(xAccelData > 3000){
           xPosition = xPosition + velocity;

       }
       else if(xAccelData < 2000 && xAccelData > -2000){
            xPosition = xPosition; //dont move
       }
       else if(xAccelData < -14000){
           xPosition = xPosition - 5*velocity;   //move left
       }
       else if(xAccelData <-9000 ){
           xPosition = xPosition - 4*velocity;
       }
       else if(xAccelData < -7000){

           xPosition = xPosition - 3*velocity;
       }
       else if(xAccelData < -5000){

           xPosition = xPosition -2*velocity;
       }
       else if(xAccelData < -3000){
           xPosition = xPosition - velocity;
       }

       if(yAccelData > 10000){
           yPosition = yPosition - 5*velocity;        //move ball up

       }
       else if(yAccelData > 8500){
           yPosition = yPosition - 4*velocity;
       }
       else if(yAccelData > 7000){
           yPosition = yPosition - 3*velocity;
       }
       else if(yAccelData > 5000){
           yPosition = yPosition - 2*velocity;
       }
       else if(yAccelData > 3000){
           yPosition = yPosition - velocity;
       }
       else if(yAccelData < 2000 && yAccelData > -2000){
           yPosition = yPosition;
       }
       else if(yAccelData < -10000){
           yPosition = yPosition + 5*velocity;
                           //move ball down
       }
       else if(yAccelData < -8500){
           yPosition = yPosition +4*velocity;
       }
       else if(yAccelData < -7000){
           yPosition = yPosition + 3*velocity;
       }
       else if(yAccelData < -5000){
           yPosition = yPosition + 2*velocity;
       }
       else if(yAccelData < -3000){
           yPosition = yPosition + velocity;
       }

       G8RTOS_semaphore_signal(&accel);


       /* wrap ball around screen */

       if(xPosition > MAX_SCREEN_X-1){
           xPosition = 2;
       }
       if(yPosition > MAX_SCREEN_Y-1){
           yPosition = 2;
       }

       if(xPosition < 0){

           xPosition = MAX_SCREEN_X-3;
       }

       if(yPosition < 0){
           yPosition = MAX_SCREEN_Y-3;

       }

        ballPtr->position.x = xPosition;
        ballPtr->position.y = yPosition;

        G8RTOS_semaphore_wait(&LCD);

        /* update the ball on LCD */
        updateBall(&prevBall,  ballPtr );

        /* signal semaphores */
        G8RTOS_semaphore_signal(&LCD);

        G8RTOS_semaphore_signal(&ballPtr->memberVarSem);

        G8RTOS_semaphore_signal(&ballArraySem);

        G8RTOS_semaphore_signal(&indexOfBall);


       G8RTOS_thread_sleep(30); //sleep

   }


}


