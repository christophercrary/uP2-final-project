/************************************************************************************
 *  File name: uP2_i2c.c
 *  Modified: 02 Septemeber 2017
 *  Author:  Christopher Crary
 *  Purpose: To define useful functions for the eUSCI_B2 I2C system on the MSP432.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "msp.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
//header guard
#ifndef UP2_I2C_H
#define UP2_I2C_H
////////////////////////////////END OF DEFINES///////////////////////////////////////


//////////////////////////////////INITIALIZATIONS////////////////////////////////////
typedef enum{ i2c_err = -1, no_i2c_err = 0 }i2c_response_code_t;      // i2c error codes
/////////////////////////////END OF INITIALIZATIONS//////////////////////////////////

////////////////////////////////INLINE FUNCTIONS/////////////////////////////////////
////////////////////////////END OF INLINE FUNCTIONS//////////////////////////////////


///////////////////////////////FUNCTION PROTOTYPES//////////////////////////////////

/************************************************************************************
* Name: i2c2_write
* Purpose: To write any number of bytes from a slave device (0 <= num_data_bytes <= 255).
*          Note: it is necessary to make a temporary copy of the passed-in data pointer
*          See 24.3.4.2.2 of the MSP432 Technical Manual for more details
* Input(s):uint16_t slave_address, uint8_t register_address, uint8_t *data, uint8_t num_data_bytes
* Output: i2c_response_code_t response_code
************************************************************************************/
i2c_response_code_t i2c2_write(uint16_t slave_address, uint8_t register_address, uint8_t *data, uint8_t num_data_bytes);

/************************************************************************************
* Name: i2c2_read
* Purpose: To read any number of bytes from a slave device (0 <= num_data_bytes <= 255).
*          Note: it is necessary to make a temporary copy of the passed-in data pointer
*          See 24.3.4.2.2 of the MSP432 Technical Manual for more details
* Input(s):uint16_t slave_address, uint8_t register_address, uint8_t *data, uint8_t num_data_bytes
* Output: i2c_response_code_t response_code
************************************************************************************/
i2c_response_code_t i2c2_read(uint16_t slave_address, uint8_t register_address, uint8_t *data, uint8_t num_data_bytes);

////////////////////////////END OF FUNCTION PROTOTYPES//////////////////////////////

#endif      // end of header guard

////////////////////////////////END OF up2_i2c.h////////////////////////////////////
