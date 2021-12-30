/*This library contains functions that set up,
 * edit, and navigate the UI in the terminal.
 *
 *  Created on: Jun 2, 2021
 *      Author: Zarek
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

#ifndef MSP_H_
#include "msp.h"
#endif

#ifndef UART_H_
#include "uart.h"
#endif

/*Different text in the terminal*/
#define DISP_TITLE      "Mr. MultiMeter Man"
#define DISP_OPTIONS    "D-DC | A-AC"
#define DISP_BAR_LABEL  "0V        1V        2V        3V"
#define DISP_MSG_1      "Mr. MultiMeter Man is your Main Method"
#define DISP_MSG_2      "Man for Measuring voltage & frequency!"

/*Locations on the terminal*/
#define DISP_TOPLEFT    "[2;2H"

#define DISP_DC_BOX     "[3;3H"
#define DISP_VAVG_LABEL "[5;4H"
#define DISP_VAVG_VAL   "[5;10H"
#define DISP_DC_BAR     "[4;21H"
#define DISP_DC_LABEL   "[5;20H"
#define DISP_DC_PLOT    "[3;18H"

#define DISP_AC_BOX     "[7;3H"
#define DISP_VRMS_LABEL "[9;4H"
#define DISP_VRMS_VAL   "[9;10H"
#define DISP_VPP_LABEL  "[11;4H"
#define DISP_VPP_VAL    "[11;10H"
#define DISP_AC_BAR     "[8;21H"
#define DISP_AC_LABEL   "[9;20H"
#define DISP_AC_PLOT    "[7;18H"

#define DISP_FREQ_BOX   "[13;3H"
#define DISP_FREQ_VAL   "[15;10H"
#define DISP_OPT_LOC    "[18;3H"

#define DISP_MSG_LOC_1  "[13;20H"
#define DISP_MSG_LOC_2  "[15;20H"
#define DISP_MSG_LOC_3  "[17;20H"
#define DISP_MSG_LOC_4  "[19;20H"

/*Starting location of boarders*/
#define DISP_INFO_B1    "[2;3H"
#define DISP_INFO_B2    "[6;3H"
#define DISP_INFO_B3    "[12;3H"
#define DISP_INFO_B4    "[16;3H"
#define DISP_PLOT_B1    "[2;18H"
#define DISP_PLOT_B2    "[6;18H"
#define DISP_PLOT_B3    "[10;18H"
#define DISP_LEFT_BORD  "[3;2H"
#define DISP_MID_BORD   "[2;17H"
#define DISP_RIGHT_BORD "[3;54H"

/*Length of various borders*/
#define DISP_PLOT_LEN   36
#define DISP_BAR_LEN    30
#define DISP_INFO_LEN   14
#define DISP_RIGHT_HT   8
#define DISP_MID_HT     7

void DISP_makeUI(void);

void DISP_update_VAVG(uint16_t val);
void DISP_update_DCBAR(uint16_t val);
void DISP_clear_DC(void);

void DISP_update_VRMS(uint16_t val);
void DISP_update_VPP(uint16_t val);
void DISP_update_ACBAR(uint16_t val);
void DISP_clear_AC(void);

void DISP_update_FREQ(uint16_t val);

void DISP_printDown(uint8_t letter);
void DISP_printVolts(uint16_t val);
void DISP_printFreq(uint16_t val);

#endif /* DISPLAY_H_ */
