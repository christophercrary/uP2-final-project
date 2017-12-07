/************************************************************************************
 *  File name: pattern.h
 *  Modified: 03 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To declare the pattern structure, some cool patterns, and a function
 *           to update patterns.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "msp.h"
#include "led.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef PATTERN_H
#define PATTERN_H

// length used in LED_PATTERN struct
#define MAX_PATTERN_LENGTH        3*255     // 255 frames of three colors each allowed

#define PATTERNS_OFF              -1        // used to start programs with LEDs set to OFF

////////////////////////////////END OF DEFINES///////////////////////////////////////

//////////////////////////////////INITIALIZATIONS////////////////////////////////////
// pattern enumerations
typedef enum{ pattern_err = -1, no_pattern_err = 0 }pattern_response_code_t;      // pattern error codes

// each LED pattern will have its own structure (maximum pattern length is 765)
typedef struct LED_PATTERN{
    uint16_t frame_counter;      // used to determine which frame to output
    uint32_t frame_rate_clock_cycles;            // # of clock cycles between each frame of pattern
    uint16_t pattern_length;     // used to keep track of the frame counter
    uint16_t pattern[MAX_PATTERN_LENGTH];
}LED_PATTERN;

extern LED_PATTERN Off;
extern LED_PATTERN GrowingTriColorSweep;
extern LED_PATTERN TriColorSweep;

/////////////////////////////END OF INITIALIZATIONS//////////////////////////////////


////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////


///////////////////////////////FUNCTION PROTOTYPES///////////////////////////////////

/************************************************************************************
* Name: pattern_update
* Purpose: To output a predefined pattern of a specified length. Patterns can be
*          different lengths, and use the LED_PATTERN structure to define information
*          about each pattern. Each frame of a pattern
*          requires three 16-bit numbers: the first represents the 16-bit RED value,
*          the second the 16-bit GREEN value, and the third represents the 16-bit
*          BLUE value.
* Input(s): LED_PATTERN pattern
* Output: pattern_response_code_t response_code
************************************************************************************/
pattern_response_code_t pattern_update(LED_PATTERN* pattern);

////////////////////////////END OF FUNCTION PROTOTYPES///////////////////////////////

#endif		// end of header guard

////////////////////////////////END OF pattern.h////////////////////////////////////
