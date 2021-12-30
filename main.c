#include "msp.h"
#include "dco.h"
#include "display.h"
#include "uart.h"
#include "adc.h"
#include "compe.h"
#include <math.h>

#define SLOW_TIMER          12000
#define AVG_TIMER           2400

#define SLOW_SAMPLES        180
#define AVG_SAMPLES         10000
#define ANALYSIS_SAMPLES    500

#define MVOLT_FIVEBIT       103
#define TIMER_FREQ          24000000
#define OFFSET_ERROR        2

uint8_t output_flag, CCflag, STPflag, received;
uint16_t ADCoutput, CCoutput, min, max;
uint32_t overflow_count;
uint64_t rms;

/* - - - - - ISRs - - - - - */

/*ISR reads received UART character and saves it in a
 * global variable */
void EUSCIA0_IRQHandler (void)
{
    /*Read the character*/
    received = EUSCI_A0->RXBUF;
}

/*ISR for timerA1, CCR0. Begins ADC conversion*/
void TA1_0_IRQHandler(void)
{
    TIMER_A1->CCTL[0] &= ~(TIMER_A_CCTLN_CCIFG);   //Clear the interrupt flag
    ADC14->CTL0 |= ADC14_CTL0_SC;                  //Set the start bit
}

/*TA0 general and CCRN interrupts*/
void TA0_N_IRQHandler(void)
{
    /*TA0_CCR2 interrupt*/
    if(TIMER_A0-> CCTL[2] & TIMER_A_CCTLN_CCIFG)
    {
        TIMER_A0-> CCTL[2] &= ~(TIMER_A_CCTLN_CCIFG);       //Clear flag

        /*Capture overflow did not occur*/
        if(!(TIMER_A0-> CCTL[2] & TIMER_A_CCTLN_COV))
        {
            CCoutput = TIMER_A0->CCR[2];                    //Save output to
            CCflag = 1;                                     //Set continue flag

            if(STPflag)
                TIMER_A0->CTL &= ~(TIMER_A_CTL_MC_MASK);    // Stop counting and interrupts
            else
                overflow_count = 0; //Start counting after we get first CCR val
        }

        /*Capture overflow occurred*/
        else
            TIMER_A0-> CCTL[2] &= ~ (TIMER_A_CCTLN_COV);    // Clear Capture Overflow
    }

    /*TA0 general interrupt*/
    if(TIMER_A0-> CTL & TIMER_A_CTL_IFG)
    {
        TIMER_A0-> CTL &= ~(TIMER_A_CTL_IFG);   //Clear flag
        overflow_count++;                       //Increment overflow count
    }
}


/*ISR for ADC, saves value and sets flag*/
void ADC14_IRQHandler(void)
{
    output_flag = 1;                            //Mark flag for Vavg function
    ADC14->CLRIFGR0 |= ADC14_CLRIFGR0_CLRIFG0;  //Clear interrupt flag
    ADCoutput = ADC14->MEM[0];                  //Save most recent value
}

/*Enable interrupts*/
void ISR_enable(void)
{
    /*Interrupts used in receiving characters from the terminal*/
    EUSCI_A0->IE |= EUSCI_A_IE_RXIE;            //Enable receive interrupts
    NVIC->ISER[0] = 1 << EUSCIA0_IRQn;          //Enable interrupts in NVIC

    /*Interrupts used in TA1 (averaging)*/
    TIMER_A1->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enable timer interrupts for CCIFG
    NVIC->ISER[0] = 1 << TA1_0_IRQn;            //TA1_CCTL0.CCIFG is 11th in the list

    /*Interrupts used in TA0 (timing square wave)*/
    TIMER_A0->CTL |= TIMER_A_CTL_IE;            //Enable TA0 general interrupts
    TIMER_A0->CCTL[2] |= (TIMER_A_CCTLN_CCIE);  //Enable Capture interrupts in TA0_CCR2
    NVIC->ISER[0] = 1 << TA0_N_IRQn;            // Timer A Interrupt

    /*Interrupts used to read value in ADC14*/
    ADC14->IER0 |= ADC14_IER0_IE0;              //Enable ADC conv complete interrupt
    NVIC->ISER[0] = 1 << ADC14_IRQn;            //Enable ADC interrupts in NVIC

    __enable_irq();                             //Enable Global interrupts

    /*Enable conversions in ADC*/
    ADC14->CTL0 |= ADC14_CTL0_ENC;
}

/* - - - - - Timer Inits - - - - - */

/*This function sets up TA1 to count to 2400. This is done 10,000 times
 * so that we take one full second to */
void average_Timer_Init(void)
{
    /*Set up TimerA*/
    TIMER_A1->CTL = ( TIMER_A_CTL_SSEL__SMCLK   |   //Select SMCLK at 24MHz
                      TIMER_A_CTL_MC__STOP);        //Halt timer until needed
}

/*This function initializes TA0_CCR2 to capture the rising edge of the comparator*/
void frequency_Timer_Init(void)
{
    TIMER_A0->CTL = ( TIMER_A_CTL_TASSEL_2   |      // SMCLK source
                     TIMER_A_CTL_ID_0        |
                     TIMER_A_EX0_TAIDEX_0    |      // No dividers
                     TIMER_A_CTL_MC__STOP);         // Stop Mode

    TIMER_A0->CCTL[2] = (  TIMER_A_CCTLN_CM__RISING  |      // Capture on rising edge
                           TIMER_A_CCTLN_CCIS__CCIB  |      // Capture/compare input select CCIxB
                           TIMER_A_CCTLN_CAP);              // Capture Mode
}

/* - - - - - General Functions - - - - - */

/*This function calculates a quick average
 * when the multimeter is in DC mode.*/
uint16_t find_Fast_Average(void)
{
    uint8_t i;
    uint32_t average;

    TIMER_A1->R = 0;                                //Reset timer count
    TIMER_A1->CCR[0] = SLOW_TIMER;                  //Have timer count for 0.5ms
    TIMER_A1->CCTL[0] &= ~(TIMER_A_CCTLN_CCIFG);    //Clear the interrupt flag
    TIMER_A1->CTL |= TIMER_A_CTL_MC__UP;            //Have timer count up

    average = 0;    //Reset average

    /*Loop 180 times*/
    for(i = 0; i < SLOW_SAMPLES; i++)
    {
        while(!(output_flag));                  //Wait to capture next value
        average += ADC14_calibrate(ADCoutput);  //Convert value to voltage, add to average
        output_flag = 0;                        //Unset flag
    }

    TIMER_A1->CTL &= ~(TIMER_A_CTL_MC_MASK);    //Set timer in stop mode

    average /= SLOW_SAMPLES;   //Divide by total samples to get average

    return average;
}

/*This function is called to calculate the
 * average voltage of the input signal.
 *
 * It will take 10,000 samples over the
 * course of a second, and average it*/
uint16_t find_Offset(void)
{
    uint16_t i, value;
    uint32_t average;

    TIMER_A1->R = 0;                                //Reset timer count
    TIMER_A1->CCR[0] = AVG_TIMER;                   //Have timer count up to 2400
    TIMER_A1->CCTL[0] &= ~(TIMER_A_CCTLN_CCIFG);    //Clear the interrupt flag
    TIMER_A1->CTL |= TIMER_A_CTL_MC__UP;            //Have timer count up

    average = 0;    //Reset average

    /*Loop 10,000 times*/
    for(i = 0; i < AVG_SAMPLES; i++)
    {
        while(!(output_flag));                  //Wait to capture next value
        value = ADC14_calibrate(ADCoutput);     //Convert value to voltage
        average += value;                       //Add val into average
        output_flag = 0;                        //Unset flag
    }

    TIMER_A1->CTL &= ~(TIMER_A_CTL_MC_MASK);    //Set timer in stop mode

    average /= AVG_SAMPLES;   //Divide by total samples to get average

    return average;
}

/*This function times the rising edge of the comparator rising edge
 * and calculates the frequency of the input wave*/
uint16_t calculate_frequency(void)
{
    uint16_t value1, value2, diff;
    uint32_t value;

    /*Initialize variables used*/
    CCflag = 0;
    STPflag = 0;

    WDT_A->CTL = WDT_A_CTL_PW           |   //Watchdog password
                 WDT_A_CTL_SSEL__SMCLK  |   //SMCLK source
                 WDT_A_CTL_IS_1;            //Reset after 5 seconds


    TIMER_A0->R = 0;                                //Reset Timer A0 count
    TIMER_A0->CCTL[2] &= ~(TIMER_A_CCTLN_CCIFG);    //Clear Capture flag
    TIMER_A0->CTL &= ~(TIMER_A_CTL_IFG);            //Clear TA0 General flag
    TIMER_A0-> CCTL[2] &= ~ (TIMER_A_CCTLN_COV);    //Clear capture overflow just in case
    TIMER_A0->CTL |= (TIMER_A_CTL_MC__CONTINUOUS);  //Count up continuously

    while(!CCflag);     //Wait until we capture CCR value
    value1 = CCoutput;  //Save output into variable
    CCflag = 0;         //Reset continue flag
    STPflag = 1;        //Notify ISR next value is last

    while(!CCflag);     //Wait until we capture CCR value
    value2 = CCoutput;  //Save output into variable

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     //Stop watchdog timer

    diff = value2 - value1;

    if(value2 < value1)
        overflow_count = ((overflow_count - 1) << 16);
    else
        overflow_count = (overflow_count << 16);

    value = diff + overflow_count;

    return (TIMER_FREQ/value) + 1;
}

/*This function analyzes the input waveform. After
 * taking 600 samples over the course of one period
 * (as determined by the measured frequency), the
 * RMS, min, and max voltages are calculated*/
void analyze_wave(uint16_t freq)
{
    uint16_t i, values[ANALYSIS_SAMPLES];
    uint32_t CCcount;

    /*Calculate value CCR to count to*/
    CCcount = (TIMER_FREQ / freq);
    CCcount /= ANALYSIS_SAMPLES;

    TIMER_A1->R = 0;                                //Reset timer count
    TIMER_A1->CCR[0] = CCcount;                     //Have timer count up to 2400
    TIMER_A1->CCTL[0] &= ~(TIMER_A_CCTLN_CCIFG);    //Clear the interrupt flag
    TIMER_A1->CTL |= TIMER_A_CTL_MC__UP;            //Have timer count up

    /*Loop 600 times, save into array for sampling*/
    for(i = 0; i < ANALYSIS_SAMPLES; i++)
    {
        while(!(output_flag));      //Wait to capture next value
        values[i] = ADCoutput;      //Save value into array
        output_flag = 0;            //Unset flag
    }

    TIMER_A1->CTL &= ~(TIMER_A_CTL_MC_MASK);    //Set timer in stop mode

    /*Initialize voltages*/
    min = values[0];
    max = values[0];
    rms = 0;

    /*Iterate through values again*/
    for(i = 0; i < ANALYSIS_SAMPLES; i++)
    {
        /*Grab new max and min values*/
        if(values[i] < min)
            min = values[i];

        if(values[i] > max)
            max = values[i];

        /*Save squares*/
        rms += (values[i] * values[i]);
    }

    rms /= ANALYSIS_SAMPLES;    //divide by number of samples

    rms = sqrt(rms); //Take the square root

    /*Convert values to voltages*/
    min = ADC14_calibrate(min);
    max = ADC14_calibrate(max);
    rms = ADC14_calibrate(rms);
}

/**
 * main.c
 */
void main(void)
{
    uint8_t FiveBitLo, FiveBitHi, i;
    uint16_t offset, freq, Vpp;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		    //Stop watchdog timer

	/*Set system clock to 24MHz*/
	set_DCO(FREQ_24_MHz);

    typedef enum {
        DC,
        AC,
        TO_AC,
        TO_DC
    } states;

    states state = DC;

	/*Initializer peripherals*/
	UART_init();
    ADC14_init();
    COMPE_init();
    average_Timer_Init();
    frequency_Timer_Init();

	DISP_makeUI();

    /*Initialize global variables*/
    output_flag = 0;

    /*Enable interrupts*/
    ISR_enable();

	while(1)
	{
	    switch(state)
	    {
	        /*DC Mode, find an average over the course of 90 ms
	         * Display this average on the terminal in the given
	         * field.*/
	        case DC:
	            offset = find_Fast_Average();   //Calculate average

	            DISP_update_VAVG(offset);       //Update Vavg in UI

	            DISP_update_DCBAR(offset);      //Update the plot

                /*Check for input character*/
                if(received == 'A')
                    state = TO_AC;

	            break;

	        /*AC Mode, find a stable offset, calculate threshold
	         * voltages for the comparator, measure frequency of
	         * the comparator output, use this frequency to calculate
	         * Vrms and Vpp, display frequency, Vrms, and Vpp in their
	         * respective fields*/
	        case AC:
	            offset = 0;

	            for(i = 0; i < 3; i++)
	                offset += find_Offset();

	            offset /= 3;

	            /*Calculate threshold voltages*/
	            FiveBitLo = ((offset * 10) / MVOLT_FIVEBIT);       //High to low
	            FiveBitHi = ((offset * 10) / MVOLT_FIVEBIT) + 1;   //Low to high

	            COMPE_change_thresh(FiveBitLo, FiveBitHi);      //Change the threshold voltages

	            freq = calculate_frequency();   //Calculate frequency
	            DISP_update_FREQ(freq);         //Update frequency on UI

	            analyze_wave(freq);     //Analyze period of wave at measured frequency

	            Vpp = max - min;    //Calculate peak to peak voltage

	            /*Update UI with new values*/
	            DISP_update_VRMS(rms);
	            DISP_update_VPP(Vpp);
	            DISP_update_ACBAR(rms);

	            /*Check for input character*/
	            if(received == 'D')
	                state = TO_DC;

	            break;

	        /*Transition from AC to DC mode, clears AC fields*/
	        case TO_DC:
	            DISP_clear_AC();
	            state = DC;
	            break;

	        /*Transition from DC to AC mode, clears DC fields*/
	        case TO_AC:
	            DISP_clear_DC();
	            state = AC;
	            break;

	        default:
	            state = DC;
	    }
	}
}
