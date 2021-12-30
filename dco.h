/*Small library for interacting with the DCO clock
 *
 *  Created on: Apr 25, 2021
 *      Author: Arsalan M., Zarek L.
 */

#ifndef DCO_H_
#define DCO_H_

#ifndef MSP_H_
#include "msp.h"
#endif

/*Macros for abstracting away the DCORSEL*/
#define FREQ_1_5_MHz    CS_CTL0_DCORSEL_0
#define FREQ_3_MHz      CS_CTL0_DCORSEL_1
#define FREQ_6_MHz      CS_CTL0_DCORSEL_2
#define FREQ_12_MHz     CS_CTL0_DCORSEL_3
#define FREQ_24_MHz     CS_CTL0_DCORSEL_4

/*Function prototypes*/
void set_DCO(int freq);

#endif /* DCO_H_ */
