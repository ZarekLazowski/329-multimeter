#include "dco.h"

void set_DCO(int freq)
{
    CS->KEY = CS_KEY_VAL; //unlock CS
    CS->CTL0 = freq;
    CS->CTL1 |= CS_CTL1_DIVM__1 | CS_CTL1_SELM__DCOCLK; // set MCLK to DCO;C
    CS->KEY = 0; // lock CS
}
