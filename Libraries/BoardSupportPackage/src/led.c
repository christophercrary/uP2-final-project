/************************************************************************************
 *  File name: led.c
 *  Modified: 02 September 2017
 *  Author:  Christopher Crary
 *  Purpose: To define useful functions for the LP3943 LED drivers, used with the
 *           MSP432.
************************************************************************************/

///////////////////////////////////INCLUDES//////////////////////////////////////////
#include "led.h"
#include "msp.h"
///////////////////////////////END OF INCLUDES///////////////////////////////////////

////////////////////////////////////DEFINES//////////////////////////////////////////
////////////////////////////////END OF DEFINES///////////////////////////////////////

//////////////////////////////////INITIALIZATIONS////////////////////////////////////
/////////////////////////////END OF INITIALIZATIONS//////////////////////////////////

///////////////////////////////FUNCTION DEFINITIONS//////////////////////////////////

/************************************************************************************
* Name: leds_init
* Purpose: To initialize all LP3943 LED drivers (for RED, GREEN, and BLUE LEDs)
*          with I2C. All LEDs are initalized to "OFF". This function makes a call to
*          leds_update
* Input(s): N/A
* Output: led_response_code_t response_code
************************************************************************************/
void leds_init()
{
    // 1. disable the I2C module (UCBxCTLW0)
    EUSCI_B2->CTLW0 = EUSCI_B_CTLW0_SWRST;

    // 2. configure MSP432 to be i2c master using 7-bit slave address
    //    and select appropriate clock (SMCLK)
    UCB2CTLW0 |= ( UCMST | UCMODE_3 | UCSYNC | UCSSEL_3 | UCTR );

    // 3. set the I2C clock speed (UCBxBRW)
    EUSCI_B2->BRW = 30;   // set clock prescaler 48MHz / 4 / 30 = 100KHz

    // 4. configure the necessary GPIO pins to act as SDA/SCL
    P3->DIR |= (SCL | SDA);     // CONFIGURE PINS AS OUTPUTS!!!!!!
    P3->SEL0 |= (SCL | SDA);
    P3->SEL1 &= ~(SCL | SDA);

    // 5. enable the I2C module (UCBxCTLW0)
    EUSCI_B2->CTLW0 &= ~(EUSCI_B_CTLW0_SWRST);

    // 6. initialize all LEDs to 'OFF'
    leds_update(RED, 0x0000);
    leds_update(GREEN, 0x0000);
    leds_update(BLUE, 0x0000);

    return;

}

/************************************************************************************
* Name: leds_update
* Purpose: To initialize update all LP3943 LED drivers (for RED, GREEN, and BLUE LEDs)
*          with I2C.
* Input(s): led_t led_driver, uint16_t leds
* Output: N/A
************************************************************************************/
void leds_update(led_t led_driver, uint16_t leds)
{
    uint8_t register_address;       // register to activate specified ranges of LEDs within drivers
    uint8_t data[4];        // data to be sent to LED driver

    register_address = (AUTO_INCREMENT | LEDS0_3);     // access all four LED registers continuously
                                                       // AUTO_INCREMENT defined as 0x10 (see LP3943 datasheet)

    // for each set of four LEDs
    for (uint8_t register_counter = 0; register_counter < 4; register_counter++)
    {
        data[register_counter] = 0;

        // for each LED
        for (uint8_t bit_counter = 0; bit_counter < 4; bit_counter++)
        {
            // if supposed to be turned on
            if (leds & (1<<15))
            {
                // turn on LED
                // NOTE: order of LEDs are reversed
                data[register_counter] |= ( 0x80 >> ( ((4-bit_counter)<<1)-1 ) );
            }
            leds = leds<<1;
        }
    }

    // configure the I2C slave address
    EUSCI_B2->I2CSA = led_driver;

    // transmit START condition (slave address will get transmitted)
    EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_TXSTT;

    // wait for the complete slave address to be sent
    // it is NECESSARY to wait for the start flag to clear
    // it will NOT work to check to STT flag in the IFG register
    while (EUSCI_B2->CTLW0 & EUSCI_B_CTLW0_TXSTT);

    // configure specific peripheral register address
    EUSCI_B2->TXBUF = register_address;

    // wait for transmit buffer to become empty
    while (!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG));

    // transmit num_data_bytes amount of bytes to slave device
    for (int byte_counter = 0; byte_counter < 4; byte_counter++)
    {
        EUSCI_B2->TXBUF = (uint16_t)(data[byte_counter]);    // post increment
        while (!(EUSCI_B2->IFG & EUSCI_B_IFG_TXIFG));
    }

    // it is likely that the slave clock is being stretched here (verify this)
    // send STOP to slave device, stop will be acknowledged immediately
    EUSCI_B2->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    // wait until STOP is sent to slave device
    // TXSTP bit in CTLW0 will be cleared when STOP is acknowledged
    while (EUSCI_B2->CTLW0 & EUSCI_B_CTLW0_TXSTP);

    return;
}
////////////////////////////END OF FUNCTION DEFINITIONS//////////////////////////////

////////////////////////////////END OF led.c////////////////////////////////////
