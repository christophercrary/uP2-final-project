/************************************************************************************
 *  File name: pattern.h
 *  Modified: 03 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To define some cool patterns, and a function to update patterns
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "msp.h"
#include "pattern.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
////////////////////////////////END OF DEFINES///////////////////////////////////////

//////////////////////////////////INITIALIZATIONS////////////////////////////////////

// global LED_PATTERN structures

LED_PATTERN Off  = {.frame_counter = 0,
                    .frame_rate_clock_cycles = 0,
                    .pattern_length = 3,
                    .pattern = {0x0000, 0x0000, 0x0000}
                    };

LED_PATTERN GrowingTriColorSweep = {.frame_counter = 0,
                                    .frame_rate_clock_cycles = 750000,
                                    .pattern_length = 3*48,
                                    .pattern = {  0x8001, 0x0000, 0x0000,
                                                  0x0000, 0x4002, 0x0000,
                                                  0x0000, 0x0000, 0x2004,
                                                  0x1008, 0x0000, 0x0000,
                                                  0x0000, 0x0810, 0x0000,
                                                  0x0000, 0x0000, 0x0420,
                                                  0x0240, 0x0000, 0x0000,
                                                  0x0000, 0x0180, 0x0000,
                                                  0x0000, 0x0000, 0xC003,
                                                  0x6006, 0x0000, 0x0000,
                                                  0x0000, 0x300C, 0x0000,
                                                  0x0000, 0x0000, 0x1818,
                                                  0x0C30, 0x0000, 0x0000,
                                                  0x0000, 0x0660, 0x0000,
                                                  0x0000, 0x0000, 0x03C0,
                                                  0xE007, 0x0000, 0x0000,
                                                  0x0000, 0x700E, 0x0000,
                                                  0x0000, 0x0000, 0x381C,
                                                  0x1C38, 0x0000, 0x0000,
                                                  0x0000, 0x0E70, 0x0000,
                                                  0x0000, 0x0000, 0x07E0,
                                                  0xF00F, 0x0000, 0x0000,
                                                  0x0000, 0x781E, 0x0000,
                                                  0x0000, 0x0000, 0x3C3C,
                                                  0x1E78, 0x0000, 0x0000,
                                                  0x0000, 0x0FF0, 0x0000,
                                                  0x0000, 0x0000, 0xF81F,
                                                  0x7C3E, 0x0000, 0x0000,
                                                  0x0000, 0x3E7C, 0x0000,
                                                  0x0000, 0x0000, 0x1FF8,
                                                  0xFC3F, 0x0000, 0x0000,
                                                  0x0000, 0x7E7E, 0x0000,
                                                  0x0000, 0x0000, 0x3FFC,
                                                  0xFE7F, 0x0000, 0x0000,
                                                  0x0000, 0x7FFE, 0x0000,
                                                  0x0000, 0x0000, 0xFFFF,
                                                  0xFFFF, 0x0000, 0x0000,
                                                  0x0000, 0xFFFF, 0x0000,
                                                  0x0000, 0x0000, 0xFFFF,
                                                  0xFFFF, 0x0000, 0x0000,
                                                  0x0000, 0xFFFF, 0x0000,
                                                  0x0000, 0x0000, 0xFFFF,
                                                  0xFFFF, 0x0000, 0x0000,
                                                  0x0000, 0xFFFF, 0x0000,
                                                  0x0000, 0x0000, 0xFFFF,
                                                  0xFFFF, 0x0000, 0x0000,
                                                  0x0000, 0xFFFF, 0x0000,
                                                  0x0000, 0x0000, 0xFFFF
                                                                          }
                                      };

LED_PATTERN TriColorSweep = {.frame_counter = 0,
                             .frame_rate_clock_cycles = 60000,
                             .pattern_length = 3*192,
                             .pattern = {   0x8000, 0x0000, 0x0000,
                                            0xC000, 0x0000, 0x0000,
                                            0xE000, 0x0000, 0x0000,
                                            0xF000, 0x0000, 0x0000,
                                            0xF800, 0x0000, 0x0000,
                                            0xFC00, 0x0000, 0x0000,
                                            0xFE00, 0x0000, 0x0000,
                                            0xFF00, 0x0000, 0x0000,
                                            0xFF80, 0x0000, 0x0000,
                                            0xFFC0, 0x0000, 0x0000,
                                            0xFFE0, 0x0000, 0x0000,
                                            0xFFF0, 0x0000, 0x0000,
                                            0xFFF8, 0x0000, 0x0000,
                                            0xFFFC, 0x0000, 0x0000,
                                            0xFFFE, 0x0000, 0x0000,
                                            0xFFFF, 0x0000, 0x0000,
                                            0x7FFF, 0x0000, 0x0000,
                                            0x3FFF, 0x0000, 0x0000,
                                            0x1FFF, 0x0000, 0x0000,
                                            0x0FFF, 0x0000, 0x0000,
                                            0x07FF, 0x0000, 0x0000,
                                            0x03FF, 0x0000, 0x0000,
                                            0x01FF, 0x0000, 0x0000,
                                            0x00FF, 0x0000, 0x0000,
                                            0x007F, 0x0000, 0x0000,
                                            0x003F, 0x0000, 0x0000,
                                            0x001F, 0x0000, 0x0000,
                                            0x000F, 0x0000, 0x0000,
                                            0x0007, 0x0000, 0x0000,
                                            0x0003, 0x0000, 0x0000,
                                            0x0001, 0x0000, 0x0000,
                                            0x0000, 0x0000, 0x0000,
                                            0x0001, 0x0001, 0x0001,
                                            0x0003, 0x0003, 0x0003,
                                            0x0007, 0x0007, 0x0007,
                                            0x000F, 0x000F, 0x000F,
                                            0x001F, 0x001F, 0x001F,
                                            0x003F, 0x003F, 0x003F,
                                            0x007F, 0x007F, 0x007F,
                                            0x00FF, 0x00FF, 0x00FF,
                                            0x01FF, 0x01FF, 0x01FF,
                                            0x03FF, 0x03FF, 0x03FF,
                                            0x07FF, 0x07FF, 0x07FF,
                                            0x0FFF, 0x0FFF, 0x0FFF,
                                            0x1FFF, 0x1FFF, 0x1FFF,
                                            0x3FFF, 0x3FFF, 0x3FFF,
                                            0x7FFF, 0x7FFF, 0x7FFF,
                                            0xFFFF, 0xFFFF, 0xFFFF,
                                            0xFFFE, 0xFFFE, 0xFFFE,
                                            0xFFFC, 0xFFFC, 0xFFFC,
                                            0xFFF8, 0xFFF8, 0xFFF8,
                                            0xFFF0, 0xFFF0, 0xFFF0,
                                            0xFFE0, 0xFFE0, 0xFFE0,
                                            0xFFC0, 0xFFC0, 0xFFC0,
                                            0xFF80, 0xFF80, 0xFF80,
                                            0xFF00, 0xFF00, 0xFF00,
                                            0xFE00, 0xFE00, 0xFE00,
                                            0xFC00, 0xFC00, 0xFC00,
                                            0xF800, 0xF800, 0xF800,
                                            0xF000, 0xF000, 0xF000,
                                            0xE000, 0xE000, 0xE000,
                                            0xC000, 0xC000, 0xC000,
                                            0x8000, 0x8000, 0x8000,
                                            0x0000, 0x0000, 0x0000,
                                            0x0000, 0x0000, 0x8000,
                                            0x0000, 0x0000, 0xC000,
                                            0x0000, 0x0000, 0xE000,
                                            0x0000, 0x0000, 0xF000,
                                            0x0000, 0x0000, 0xF800,
                                            0x0000, 0x0000, 0xFC00,
                                            0x0000, 0x0000, 0xFE00,
                                            0x0000, 0x0000, 0xFF00,
                                            0x0000, 0x0000, 0xFF80,
                                            0x0000, 0x0000, 0xFFC0,
                                            0x0000, 0x0000, 0xFFE0,
                                            0x0000, 0x0000, 0xFFF0,
                                            0x0000, 0x0000, 0xFFF8,
                                            0x0000, 0x0000, 0xFFFC,
                                            0x0000, 0x0000, 0xFFFE,
                                            0x0000, 0x0000, 0xFFFF,
                                            0x0000, 0x0000, 0x7FFF,
                                            0x0000, 0x0000, 0x3FFF,
                                            0x0000, 0x0000, 0x1FFF,
                                            0x0000, 0x0000, 0x0FFF,
                                            0x0000, 0x0000, 0x07FF,
                                            0x0000, 0x0000, 0x03FF,
                                            0x0000, 0x0000, 0x01FF,
                                            0x0000, 0x0000, 0x00FF,
                                            0x0000, 0x0000, 0x007F,
                                            0x0000, 0x0000, 0x003F,
                                            0x0000, 0x0000, 0x001F,
                                            0x0000, 0x0000, 0x000F,
                                            0x0000, 0x0000, 0x0007,
                                            0x0000, 0x0000, 0x0003,
                                            0x0000, 0x0000, 0x0001,
                                            0x0000, 0x0000, 0x0000,
                                            0x0001, 0x0000, 0x0000,
                                            0x0003, 0x0000, 0x0000,
                                            0x0007, 0x0000, 0x0000,
                                            0x000F, 0x0000, 0x0000,
                                            0x001F, 0x0000, 0x0000,
                                            0x003F, 0x0000, 0x0000,
                                            0x007F, 0x0000, 0x0000,
                                            0x00FF, 0x0000, 0x0000,
                                            0x01FF, 0x0000, 0x0000,
                                            0x03FF, 0x0000, 0x0000,
                                            0x07FF, 0x0000, 0x0000,
                                            0x0FFF, 0x0000, 0x0000,
                                            0x1FFF, 0x0000, 0x0000,
                                            0x3FFF, 0x0000, 0x0000,
                                            0x7FFF, 0x0000, 0x0000,
                                            0xFFFF, 0x0000, 0x0000,
                                            0xFFFE, 0x0000, 0x0000,
                                            0xFFFC, 0x0000, 0x0000,
                                            0xFFF8, 0x0000, 0x0000,
                                            0xFFF0, 0x0000, 0x0000,
                                            0xFFE0, 0x0000, 0x0000,
                                            0xFFC0, 0x0000, 0x0000,
                                            0xFF80, 0x0000, 0x0000,
                                            0xFF00, 0x0000, 0x0000,
                                            0xFE00, 0x0000, 0x0000,
                                            0xFC00, 0x0000, 0x0000,
                                            0xF800, 0x0000, 0x0000,
                                            0xF000, 0x0000, 0x0000,
                                            0xE000, 0x0000, 0x0000,
                                            0xC000, 0x0000, 0x0000,
                                            0x8000, 0x0000, 0x0000,
                                            0x0000, 0x0000, 0x0000,
                                            0x8000, 0x8000, 0x8000,
                                            0xC000, 0xC000, 0xC000,
                                            0xE000, 0xE000, 0xE000,
                                            0xF000, 0xF000, 0xF000,
                                            0xF800, 0xF800, 0xF800,
                                            0xFC00, 0xFC00, 0xFC00,
                                            0xFE00, 0xFE00, 0xFE00,
                                            0xFF00, 0xFF00, 0xFF00,
                                            0xFF80, 0xFF80, 0xFF80,
                                            0xFFC0, 0xFFC0, 0xFFC0,
                                            0xFFE0, 0xFFE0, 0xFFE0,
                                            0xFFF0, 0xFFF0, 0xFFF0,
                                            0xFFF8, 0xFFF8, 0xFFF8,
                                            0xFFFC, 0xFFFC, 0xFFFC,
                                            0xFFFE, 0xFFFE, 0xFFFE,
                                            0xFFFF, 0xFFFF, 0xFFFF,
                                            0x7FFF, 0x7FFF, 0x7FFF,
                                            0x3FFF, 0x3FFF, 0x3FFF,
                                            0x1FFF, 0x1FFF, 0x1FFF,
                                            0x0FFF, 0x0FFF, 0x0FFF,
                                            0x07FF, 0x07FF, 0x07FF,
                                            0x03FF, 0x03FF, 0x03FF,
                                            0x01FF, 0x01FF, 0x01FF,
                                            0x00FF, 0x00FF, 0x00FF,
                                            0x007F, 0x007F, 0x007F,
                                            0x003F, 0x003F, 0x003F,
                                            0x001F, 0x001F, 0x001F,
                                            0x000F, 0x000F, 0x000F,
                                            0x0007, 0x0007, 0x0007,
                                            0x0003, 0x0003, 0x0003,
                                            0x0001, 0x0001, 0x0001,
                                            0x0000, 0x0000, 0x0000,
                                            0x0000, 0x0000, 0x0001,
                                            0x0000, 0x0000, 0x0003,
                                            0x0000, 0x0000, 0x0007,
                                            0x0000, 0x0000, 0x000F,
                                            0x0000, 0x0000, 0x001F,
                                            0x0000, 0x0000, 0x003F,
                                            0x0000, 0x0000, 0x007F,
                                            0x0000, 0x0000, 0x00FF,
                                            0x0000, 0x0000, 0x01FF,
                                            0x0000, 0x0000, 0x03FF,
                                            0x0000, 0x0000, 0x07FF,
                                            0x0000, 0x0000, 0x0FFF,
                                            0x0000, 0x0000, 0x1FFF,
                                            0x0000, 0x0000, 0x3FFF,
                                            0x0000, 0x0000, 0x7FFF,
                                            0x0000, 0x0000, 0xFFFF,
                                            0x0000, 0x0000, 0xFFFE,
                                            0x0000, 0x0000, 0xFFFC,
                                            0x0000, 0x0000, 0xFFF8,
                                            0x0000, 0x0000, 0xFFF0,
                                            0x0000, 0x0000, 0xFFE0,
                                            0x0000, 0x0000, 0xFFC0,
                                            0x0000, 0x0000, 0xFF80,
                                            0x0000, 0x0000, 0xFF00,
                                            0x0000, 0x0000, 0xFE00,
                                            0x0000, 0x0000, 0xFC00,
                                            0x0000, 0x0000, 0xF800,
                                            0x0000, 0x0000, 0xF000,
                                            0x0000, 0x0000, 0xE000,
                                            0x0000, 0x0000, 0xC000,
                                            0x0000, 0x0000, 0x8000,
                                            0x0000, 0x0000, 0x0000
                                                                    }
                            };


/////////////////////////////END OF INITIALIZATIONS//////////////////////////////////


////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////


///////////////////////////////FUNCTION DEFINITION///////////////////////////////////

/************************************************************************************
* Name: pattern_update
* Purpose: To output a predefined pattern of a specified length. Patterns can be
*          different lengths, and use the LED_PATTERN structure to define information
*          about each pattern. Each frame of a pattern
*          requires three 16-bit numbers: the first represents the 16-bit RED value,
*          the second the 16-bit GREEN value, and the third represents the 16-bit
*          BLUE value.
* Input(s): LED_PATTERN* pattern
* Output: pattern_response_code_t response_code
************************************************************************************/
pattern_response_code_t pattern_update(LED_PATTERN* pattern)
{
  // if previous frame outputted was the last frame of the pattern->pattern array
  if (pattern->frame_counter == pattern->pattern_length)
  {
      pattern->frame_counter = 0;     // initial frame is the next to be outputted
  }

  leds_update(RED, pattern->pattern[(pattern->frame_counter)++]);      // activate RED LEDs of current frame
  leds_update(GREEN, pattern->pattern[(pattern->frame_counter)++]);      // activate GREEN LEDs of current frame
  leds_update(BLUE, pattern->pattern[(pattern->frame_counter)++]);      // activate BLUE LEDs of current frame

  return no_pattern_err;
}

////////////////////////////END OF FUNCTION DEFINITIONS///////////////////////////////

////////////////////////////////END OF pattern.c////////////////////////////////////
