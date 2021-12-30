/*This library offers an initialization function
 * that configures the MSP432's 14-bit ADC unit to
 * operate with a single channel, doing single conversions
 * based on values read from P5.4.
 *
 * This library uses the 3.3 V rail as a reference,
 * and samples the input for 4 clock cycles.
 *
 *  Created on: May 20, 2021
 *      Author: Zarek & Armin
 */

#ifndef ADC_H_
#define ADC_H_

#ifndef MSP_H_
#include "msp.h"
#endif

#define ADC_PORT P5
#define ADC_INPUT BIT4

/*Calibration values (y = mx + b)*/
#define ADC_M 2
#define ADC_B -280
#define ADC_ZOOM 100 //Division to get in terms of 10*millivolts

void ADC14_init(void);
uint16_t ADC14_calibrate(uint16_t num);

#endif /* ADC_H_ */
