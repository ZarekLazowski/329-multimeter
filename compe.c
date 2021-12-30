#include "compe.h"

/*This function initializes the Comparator E 0 module to take
 * an input from Analog Input 1, and compares it to voltages
 * from CE1's reference voltage generator*/
void COMPE_init(void)
{

    COMP_E0->CTL0 |= (COMP_E_CTL0_IPEN       |       // Analog input V+ terminal is enabled
                      COMP_E_CTL0_IPSEL_1);          // Choose inchannel C1 (P8.0)

    COMP_E0->CTL1 |= (COMP_E_CTL1_ON         |       // Turns comparator on
                      COMP_E_CTL1_FDLY_3     |       // Filter Delay 3000ns
                      COMP_E_CTL1_F);                // Comparator filter on

    COMP_E0->CTL2 |= (COMP_E_CTL2_RS_1        |      // VCC applied to the resistor ladder
                      COMP_E_CTL2_RSEL);             // VREF is applied to the V- terminal;

    COMP_E0->INT  |= (COMP_E_INT_IE);                // Comparator output interrupt enable

    COMPE_INPORT->SEL0 |= (COMPE_INPIN);             // Positive Select
    COMPE_INPORT->SEL1 |= (COMPE_INPIN);
}

/*This function sets new voltages for the CE0 threshold
 * voltages. This is done by cleaning the input to ensure
 * only 5 bits are written, unsetting bits in the REF0/REF1
 * portion of the control register, and setting the requested
 * bits.*/
void COMPE_change_thresh(uint8_t lo, uint8_t hi)
{
    uint8_t FiveBitLo, FiveBitHi;

    /*Clean input to be only 5-bit*/
    FiveBitLo = lo & COMPE_FIVEBITS;
    FiveBitHi = hi & COMPE_FIVEBITS;

    /*Reset reference voltage values*/
    COMP_E0->CTL2 &= ~(COMP_E_CTL2_REF1_MASK | COMP_E_CTL2_REF0_MASK);

    /*Sets up hi-to-lo and lo-to-hi voltages for CE0*/
    COMP_E0->CTL2 |= (FiveBitLo << COMP_E_CTL2_REF1_OFS);  //Reference voltage is low
    COMP_E0->CTL2 |= (FiveBitHi << COMP_E_CTL2_REF0_OFS);  //Reference voltage is High
}

/*This function will set up P7.1 as an output of the comparator
 * E 0 module, for use in debugging*/
void COMPE_debug(void)
{
    /*Setup output to analyze square wave*/
    COMPE_OUTPORT->SEL0 |=      COMPE_OUTPIN;
    COMPE_OUTPORT->SEL1 &=      ~(COMPE_OUTPIN);
    COMPE_OUTPORT->DIR  |=      COMPE_OUTPIN;
}
