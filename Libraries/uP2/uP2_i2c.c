/************************************************************************************
 *  File name: uP2_i2c.c
 *  Modified: 02 Septemeber 2017
 *  Author:  Christopher Crary
 *  Purpose: To define useful functions for the eUSCI_B2 I2C system on the MSP432.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "uP2_i2c.h"
#include "msp.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
////////////////////////////////END OF DEFINES///////////////////////////////////////

//////////////////////////////////INITIALIZATIONS////////////////////////////////////
/////////////////////////////END OF INITIALIZATIONS//////////////////////////////////

///////////////////////////////FUNCTION DEFINITIONS//////////////////////////////////

/************************************************************************************
* Name: i2c2_write
* Purpose: To write any number of bytes from a slave device (0 <= num_data_bytes <= 255).
*          Note: it is necessary to make a temporary copy of the passed-in data pointer
*          See 24.3.4.2.2 of the MSP432 Technical Manual for more details
* Input(s):uint16_t slave_address, uint8_t register_address, uint8_t *data, uint8_t num_data_bytes
* Output: i2c_response_code_t response_code
************************************************************************************/

i2c_response_code_t i2c2_write(uint16_t slave_address, uint8_t register_address, uint8_t *data, uint8_t num_data_bytes)
{
  if (num_data_bytes <= 0)
  {
    return i2c_err;   //  do not write anything to slave
  }
  // make a temporary copy of the data pointer
  uint8_t* temp = data;

  // disable the I2C module (UCBxCTLW0)
  // NOTE: IT IS NECESSARY TO DISABLE THE I2C SYSTEM BEFORE SETTING THE SLAVE ADDRESS
//  EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_SWRST;

  // clear all I2C flags
  //EUSCI_B2->IFG = 0;

  // configure the I2C slave address
  EUSCI_B2->I2CSA = slave_address;

  // re-enable the I2C module (UCBxCTLW0)
//  EUSCI_B2->CTLW0 &= ~(EUSCI_B_CTLW0_SWRST);


  // enable I2C transmitter
  // transmitter will automatically send a '0' to signify a write
//  EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_TR;

  // transmit START condition (slave address will get transmitted)
  EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_TXSTT;

  // wait for transmit buffer to become empty
  while ((EUSCI_B2->IFG & EUSCI_B_IFG_STTIFG));

  // configure specific peripheral register address
  EUSCI_B2->TXBUF = register_address;

  while (!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG));

  // transmit num_data_bytes amount of bytes to slave device
  for (int byte_counter = 0; byte_counter < num_data_bytes; byte_counter++)
  {
      EUSCI_B2->TXBUF = (uint16_t)(*temp++);    // post increment
      while (!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG));
  }
  //while (!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG));

  // it is likely that the slave clock is being stretched here (verify this)
  // send STOP to slave device, stop will be acknowledged immediately
  EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

  // wait until STOP is sent to slave device
  // TXSTP bit in CTLW0 will be cleared when STOP is acknowledged
//  while (EUSCI_B2->CTLW0 & EUSCI_B_CTLW0_TXSTP);

  // clear all I2C flags
  //EUSCI_B2->IFG = 0;

  return no_i2c_err;   // data transmission(s) complete
}

/************************************************************************************
* Name: i2c2_read
* Purpose: To read any number of bytes from a slave device (0 <= num_data_bytes <= 255).
*          Note: it is necessary to make a temporary copy of the passed-in data pointer
*          See 24.3.4.2.2 of the MSP432 Technical Manual for more details
* Input(s):uint16_t slave_address, uint8_t register_address, uint8_t *data, uint8_t num_data_bytes
* Output: i2c_response_code_t response_code
************************************************************************************/
i2c_response_code_t i2c2_read(uint16_t slave_address, uint8_t register_address, uint8_t *data, uint8_t num_data_bytes)
{
    if (num_data_bytes <= 0)
    {
        return i2c_err;   //  do not write anything to slave
    }

    uint8_t *temp = data;   // temp copy of data pointer, as to not change initial value

    // To read from slave device,
    // 1. Write slave/register addresses to slave device
    // 2. Enable I2C receiver and generate restart signal to send slave address and read signal
    // 3. Read num_data_bytes amount of bytes into a selected memory location

    // disable the I2C module (UCBxCTLW0)
    // NOTE: IT IS NECESSARY TO DISABLE THE I2C SYSTEM BEFORE SETTING THE SLAVE ADDRESS
    EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_SWRST;

    // configure the I2C slave address
    EUSCI_B2->I2CSA = slave_address;

    // re-enable the I2C module (UCBxCTLW0)
    EUSCI_B2->CTLW0 &= ~(EUSCI_B_CTLW0_SWRST);


    // enable I2C transmitter
    // transmitter will automatically send a '0' to signify a write
    EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_TR;

    // transmit START condition (slave address will get transmitted)
    EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_TXSTT;

    // wait for transmit buffer to become empty
    while (!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG));

    // access specific peripheral register address
    EUSCI_B2->TXBUF = register_address;

    // wait for transmit buffer to become empty
    while (!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG));

    // enable I2C receiver
    EUSCI_B2->CTLW0 &= ~(EUSCI_B_CTLW0_TR);

    // transmit restart signal and read signal
    EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_TXSTT;

    // wait until restart is complete
    while (!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG));

    // data is ready to be read from slave device
    for (int i = 0; i < num_data_bytes; i++)
    {
        // if the last byte is being transmitted, generate a STOP condition (IT MAY NEED TO BE THE SECOND-TO-LAST BYTE)
        // "A STOP condition is either generated by the automatic STOP generation or by setting the UCTXSTP bit.
        // "The next byte received from the slave is followed by a NACK and a STOP condition."
        if (i == (num_data_bytes - 1))
        {
            // send STOP to slave device
            // it is necessary to ensure that this bit is set before
            // receiving the last data byte,
            // as if not done before, another data byte could be read-in
            EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_TXSTP;
        }

        // wait until data is received
        while (!(EUSCI_B2->IFG & EUSCI_B_IFG_RXIFG));

        // read in data from slave device
        *temp++ = EUSCI_B2->RXBUF;
    }

    // wait until STOP is sent to slave device
    // TXSTP bit in CTLW0 will be cleared when STOP is acknowledged
    while (EUSCI_B2->CTLW0 & EUSCI_B_CTLW0_TXSTP);

    return no_i2c_err;
}
////////////////////////////END OF FUNCTION DEFINITIONS//////////////////////////////

////////////////////////////////END OF up2_i2c.c////////////////////////////////////
