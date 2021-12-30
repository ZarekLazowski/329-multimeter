#include "adc.h"

/*This function initializes the input pins for the ADC on the
 * MSP432 and configures the control register to the below options*/
void ADC14_init(void)
{
    ADC_PORT->SEL0 |= ADC_INPUT;   // Configure P5.4 for ADC analog input pin
    ADC_PORT->SEL1 |= ADC_INPUT;
    ADC_PORT->DIR &= ~(ADC_INPUT);

    ADC14->CTL0 = (ADC14_CTL0_ON            //Turns on the ADC
                |  ADC14_CTL0_SHT0_0        //Sample and hold for 4 clk cycles
                |  ADC14_CTL0_CONSEQ_0      //Single channel, single conversion
                |  ADC14_CTL0_SSEL__SMCLK   //Select SMCLK source
                |  ADC14_CTL0_DIV__1        //No clock dividing
                |  ADC14_CTL0_SHP);         //Sample and hold mode

    ADC14->CTL1 = (ADC14_CTL1_RES__14BIT);  //14 bit conversion mode

    //Single ended mode (ADC14DIF = 0) input: A1 (5.4)
    //3.3 V reference
    ADC14->MCTL[0] = (ADC14_MCTLN_INCH_1 | ADC14_MCTLN_VRSEL_0);
}

uint16_t ADC14_calibrate(uint16_t num)
{
    /*Special case: calibrated 0 to start at 334*/
    if(num < 334)
        return 0;

    /*y = mx + b*/
    else
        return ( ((ADC_M*num) + ADC_B) / ADC_ZOOM );
}
