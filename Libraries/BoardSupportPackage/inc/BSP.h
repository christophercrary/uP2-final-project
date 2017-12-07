/*
 * BSP.h
 *
 *  Modified On: 22 October, 2017
 *      Author: Raz Aloni
 *      Modified By: Chris Crary
 */

#ifndef BSP_H_
#define BSP_H_

/* Port Configuration for this Board */

/*
 * HFXT
 * 	In:				        PJ.3
 * 	Out:						PJ.2
 *
 * LED I2C
 *  SDA:						P3.6
 *	SCL:						P3.7
 *
 * SensorPack I2C
 * 	SDA:						P6.4
 * 	SCL:						P6.5
 *
 * SensorPack OPT3001
 * 	IRQ Pin:					P?.?
 *
 * SensorPack TMP007
 * 	IRQ Pin:					P?.?
 *
 * Internal ADC
 *	AnalogIn:					P6.0, 6.1
 */

/* Includes */

#include <stdint.h>
#include "bme280_support.h"
#include "bmi160_support.h"
#include "opt3001.h"
#include "tmp007.h"
#include "BackChannelUart.h"
#include "ClockSys.h"
#include "Joystick.h"
#include "led.h"
#include <driverlib.h>
#include "LCD.h"
#include "daughter_board.h"     // used to configure buttons on daughter_board


/********************************** Public Functions **************************************/

/* Initializes the entire board */
extern void BSP_InitBoard();

/********************************** Public Functions **************************************/

#endif /* BSP_H_ */
