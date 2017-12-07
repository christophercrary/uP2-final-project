/*
 * BSP.c
 *
 *  Created on: Dec 30, 2016
 *      Author: Raz Aloni
 *      Modified By: Christopher Crary
 *      Modified On: 22 October 2017
 */

#include "BSP.h"
#include <driverlib.h>
#include "i2c_driver.h"
#include "stdbool.h"      // used for boolean enum


/* Initializes the entire board */
void BSP_InitBoard()
{
	/* Disable Watchdog */
	WDT_A_clearTimer();
	WDT_A_holdTimer();

	/* Initialize Clock */
	ClockSys_SetMaxFreq();

	/* Init i2c */
	initI2C();

	/* Init Opt3001 */
	//sensorOpt3001Enable(true);

	/* Init Tmp007 */
	//sensorTmp007Enable(true);

	/* Init Bmi160 */
   // bmi160_initialize_sensor();

    /* Init joystick without interrupts */
	Joystick_Init_Without_Interrupt();

	/* Init Bme280 */
	//bme280_initialize_sensor();

	/* Init BackChannel UART */
	//BackChannelInit();

	/* Init RGB LEDs on daughter board */
	leds_init();

	/* Initialize LCD */
	LCD_Init(true);        // touch-screen interrupt enabled

	/* Initialize daughter board buttons (w/interrupts) */
	/* NOTE: The necessary PORT IRQs are not configured
	         It is the user's responsibility to create
	         the necessary aperiodic threads before using
	         the buttons. */
	button_all_int_enable(6);      // priority of 6 (lowest user priority)
}


