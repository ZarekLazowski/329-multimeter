/*This library contains macros and functions necessary
 * to utilize the UART module on the MSP432.
 *
 * The baud rate chosen for this library is 115.2 kb/s
 *
 * REQUIRES INPUT CLK OF 3MHZ
 *
 *      Author: Zarek Lazowski & Spencer Stone
 */

#ifndef UART_H_
#define UART_H_

#ifndef MSP_H_
#include "msp.h"
#endif

#define UART_BR_SCALER  13
#define UART_BR_FREQ    0
#define UART_BR_STAGE   0x25

#define UART_PORT P1
#define UART_RXD BIT2
#define UART_TXD BIT3

#define UART_ESC 0x1B

/*List of useful escape commands*/
//Movement
#define UART_U1     "[1A"   //up
#define UART_U5     "[5A"
#define UART_D1     "[1B"   //down
#define UART_D5     "[5B"
#define UART_R1     "[1C"   //right
#define UART_R5     "[5C"
#define UART_L1     "[1D"   //left
#define UART_L5     "[5D"
#define UART_HOME   "[H"    //top left

//Effects
#define UART_EOFF   "[0m"   //effects off
#define UART_CLEAR  "[2J"   //clear screen
#define UART_BLINK  "[5m"   //following text blinks

//RGB text
#define UART_CRED   "[31m"
#define UART_CGREEN "[32m"
#define UART_CBLUE  "[34m"
#define UART_CWHITE "[37m"

void UART_init(void);
void UART_esc_code(uint8_t *code);
void UART_print_str(uint8_t *str);
void UART_print_char(uint8_t letter);

#endif /* UART_H_ */
