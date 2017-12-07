/************************************************************************************
 *  File name: led.h
 *  Modified: 02 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To declare useful functions for the LP3943 LED drivers, used with the
 *           MSP432.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "msp.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef LED_H
#define LED_H

#define SCL                 (1<<7)
#define SDA                 (1<<6)
#define NUM_LED_REGISTERS   4
#define AUTO_INCREMENT      0x10        // pre-defined value in LP3943 used to write/read
                                        // multiple registers in one I2C write/read cycle

#define LEDS0_3             0x06        // see LP3943 data sheet, section 7.6.3
#define LEDS4_7             0x07
#define LEDS8_11            0x08
#define LEDS12_15           0x09

////////////////////////////////END OF DEFINES///////////////////////////////////////

//////////////////////////////////INITIALIZATIONS////////////////////////////////////
// enumerations
typedef enum{ RED = 0x60, GREEN = 0x61, BLUE = 0x62 }led_t;       // led driver slave addresses
typedef enum{ led_err = -1, no_led_err = 0 }led_response_code_t;      // led error codes

/////////////////////////////END OF INITIALIZATIONS//////////////////////////////////


////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////


///////////////////////////////FUNCTION PROTOTYPES///////////////////////////////////

/************************************************************************************
* Name: leds_init
* Purpose: To initialize all LP3943 LED drivers (for RED, GREEN, and BLUE LEDs)
*          with I2C. All LEDs are initalized to "OFF". This function makes a call to
*          leds_update.
* Input(s): N/A
* Output: led_response_code_t response_code
************************************************************************************/
void leds_init(void);

/************************************************************************************
* Name: leds_update
* Purpose: To initialize update all LP3943 LED drivers (for RED, GREEN, and BLUE LEDs)
*          with I2C.
* Input(s): led_t led_driver, uint16_t leds
* Output: N/A
************************************************************************************/
void leds_update(led_t led_driver, uint16_t leds);

////////////////////////////END OF FUNCTION PROTOTYPES///////////////////////////////

#endif		// end of header guard

////////////////////////////////END OF led.h////////////////////////////////////
