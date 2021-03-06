/************************************************************************************
 *  Final Project - MuPhone
 *  File name: main.c
 *  Modified: 22 November 2017
 *  Author:  Christopher Crary
 *  Description: This program is used to emulate a basic smartphone, MuPhone.
 *               The MuPhone is powered by the G8RTOS, a real-time operating system.
 *               The currently implemented applications on the smartphone are ...
************************************************************************************/

/////////////////////////////////DEPENDENCIES////////////////////////////////////////
#include "muphone.h"
/////////////////////////////END OF DEPENDENCIES/////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
////////////////////////////////END OF DEFINES///////////////////////////////////////

////////////////////////////////////EXTERNS//////////////////////////////////////////
////////////////////////////////END OF EXTERNS///////////////////////////////////////

//////////////////////////////PUBLIC DATA MEMBERS////////////////////////////////////
//////////////////////////END OF PUBLIC DATA MEMBERS/////////////////////////////////

//////////////////////////////PRIVATE DATA MEMBERS///////////////////////////////////
//////////////////////////END OF PRIVATE DATA MEMBERS////////////////////////////////


///////////////////////////////FUNCTION PROTOTYPES///////////////////////////////////
////////////////////////////END OF FUNCTION PROTOTYPES///////////////////////////////

//////////////////////////////////MAIN PROGRAM///////////////////////////////////////

int main(void)
{
    // initialize all necessary MuPhone components,
    // wait for device to be "turned on" (IMPLEMENT HARDWARE INTERRUPT)
    muphone();

    return 0;       // end of program (never reached)
}

///////////////////////////////END OF MAIN PROGRAM///////////////////////////////////

//////////////////////////////FUNCTION DEFINITIONS///////////////////////////////////
///////////////////////////END OF FUNCTION DEFINITIONS///////////////////////////////

////////////////////////////INTERRUPT SERVICE ROUTINES///////////////////////////////
////////////////////////END OF INTERRUPT SERVICE ROUTINES////////////////////////////

////////////////////////////////END OF main.c////////////////////////////////////
