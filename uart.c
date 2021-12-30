#include "uart.h"

/*This function initializes the MSP432's eUSCI-A
 * options to run at 115.2 kb/s and enables
 * the UART GPIO pins*/
void UART_init(void)
{
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; /*Put module into software reset*/

    /*Configure eUSCI-A*/
    EUSCI_A0->CTLW0 = ( EUSCI_A_CTLW0_SWRST         | //Software reset
                        EUSCI_A_CTLW0_MODE_0        | //Select UART mode
                        EUSCI_A_CTLW0_SSEL__SMCLK   | //Select SMCLK
                        EUSCI_A_CTLW0_SPB);           //Send two stop bits just in case

    EUSCI_A0->BRW = UART_BR_SCALER;

    EUSCI_A0->MCTLW = ( EUSCI_A_MCTLW_OS16                          | //Enable oversampling
                        (UART_BR_FREQ << EUSCI_A_MCTLW_BRF_OFS)     | //Set Baud Rate Frequency
                        (UART_BR_STAGE << EUSCI_A_MCTLW_BRS_OFS));    //Set Baud Rate Modulation Stage

    /*Configure UART pins*/
    UART_PORT->SEL0 |= (UART_RXD | UART_TXD); /*Select UART for RXD and TXD pins*/
    UART_PORT->SEL1 &= ~(UART_RXD | UART_TXD);

    EUSCI_A0->CTLW0 &= ~(EUSCI_A_CTLW0_SWRST); /*Exit software reset*/
}

void UART_esc_code(uint8_t *code)
{
    /*Send escape character to terminal*/
    UART_print_char(UART_ESC);

    UART_print_str(code);
}

void UART_print_str(uint8_t *str)
{
    int i;

    /*Continue calling the print char function with contents of string until
     * we reach a nul byte*/
    for(i = 0; str[i] != '\0'; i++)
        UART_print_char(str[i]);
}

void UART_print_char(uint8_t letter)
{
    /*Wait for transmit ifg*/
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));

    /*Transmit byte*/
    EUSCI_A0->TXBUF = letter;
}
