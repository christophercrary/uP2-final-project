#include "Applications.h"
#include "BSP.h"

#ifndef DRAWSTUFF_H
#define DRAWSTUFF_H

/************************************************************************************
 * Name: aperiodic_mumessage_draw_image
 * Purpose:
 * Input(s): N/A
 * Output: N/A
 ***********************************************************************************/
void aperiodic_mumessage_draw(void);

/************************************************************************************
* Name: thread_muphone_home_screen_check_TP
* Purpose: Thread to check if touch made to LCD TouchPanel interacted with LCD TP
*          as to draw on screen.
* Input(s): N/A
* Output: N/A
************************************************************************************/
void thread_mumessage_image_draw_check_TP(void);




#endif
