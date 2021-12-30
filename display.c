#include "display.h"

/*This function goes to the predefined location
 * to print the given Vavg*/
void DISP_update_VAVG(uint16_t val)
{
    UART_esc_code(DISP_VAVG_VAL);
    DISP_printVolts(val);
}

/*This function updates the Vavg bar graph*/
void DISP_update_DCBAR(uint16_t val)
{
    uint8_t i, boxes;

    /*Go to beginning of plot*/
    UART_esc_code(DISP_DC_BAR);

    /*Divide by 10 to figure out how many boxes are required*/
    boxes = val / 10;

    for(i = 0; i < DISP_BAR_LEN; i++)
    {
        /*Print character for every 1/10 volt*/
        if(i < boxes)
            UART_print_char('#');
        /*If no more boxes, print blanks*/
        else
            UART_print_char(' ');
    }
}

/*This function blanks out the DC portions of the
 * UI when switching to AC mode*/
void DISP_clear_DC(void)
{
    uint8_t i;

    UART_esc_code(DISP_VAVG_VAL);

    for(i = 0; i < 6; i++)
        UART_print_char(' ');

    UART_esc_code(DISP_DC_BAR);

    for(i = 0; i < DISP_BAR_LEN; i++)
        UART_print_char(' ');
}

/*This function goes to the predefined location
 * to print the given Vrms*/
void DISP_update_VRMS(uint16_t val)
{
    UART_esc_code(DISP_VRMS_VAL);
    DISP_printVolts(val);
}

/*This function goes to the predefined location
 * to print the given Vpp*/
void DISP_update_VPP(uint16_t val)
{
    UART_esc_code(DISP_VPP_VAL);
    DISP_printVolts(val);
}

/*This function updates the Vrms bar graph*/
void DISP_update_ACBAR(uint16_t val)
{
    uint8_t i, boxes;

    /*Go to beginning of plot*/
    UART_esc_code(DISP_AC_BAR);

    /*Divide by 10 to figure out how many boxes are required*/
    boxes = val / 10;

    for(i = 0; i < DISP_BAR_LEN; i++)
    {
        /*Print character for every 1/10 volt*/
        if(i < boxes)
            UART_print_char('#');
        /*If no more boxes, print blanks*/
        else
            UART_print_char(' ');
    }
}


/*This function goes to the predefined location
 * to print the given frequency*/
void DISP_update_FREQ(uint16_t val)
{
    UART_esc_code(DISP_FREQ_VAL);
    DISP_printFreq(val);
}

/*This function blanks out the DC portions of the
 * UI when switching to AC mode*/
void DISP_clear_AC(void)
{
    uint8_t i;

    UART_esc_code(DISP_VRMS_VAL);

    for(i = 0; i < 6; i++)
        UART_print_char(' ');

    UART_esc_code(DISP_VPP_VAL);

    for(i = 0; i < 6; i++)
        UART_print_char(' ');

    UART_esc_code(DISP_AC_BAR);

    for(i = 0; i < DISP_BAR_LEN; i++)
        UART_print_char(' ');
}

/*Print character and move to location one below*/
void DISP_printDown(uint8_t letter)
{
    UART_print_char(letter);    //Print char
    UART_esc_code(UART_L1);     //Go back
    UART_esc_code(UART_D1);     //Go down
}

/*Print voltage from converted value. Supplied values are in
 * terms of 10's of millivolts.*/
void DISP_printVolts(uint16_t val)
{
    uint8_t hundred, ten, one;

    hundred = (val/100);                    //Val in 100's place
    ten = (val/10) - (hundred*10);          //Val in 10's place
    one = val - (hundred*100) - (ten*10);   //Val in 1's place

    UART_print_char(hundred + 0x30);
    UART_print_char('.');
    UART_print_char(ten + 0x30);
    UART_print_char(one + 0x30);
    UART_print_str(" V");
}

/*Print given frequency.*/
void DISP_printFreq(uint16_t val)
{
    uint8_t thousand, hundred, ten, one;

    thousand = val/1000;                                            //Val in 1000's place
    hundred = (val/100) - (thousand * 10);                          //Val in 100's place
    ten = (val/10) - (thousand * 100) - (hundred * 10);             //Val in 10's place
    one = val - (thousand * 1000) - (hundred * 100) - (ten * 10);   //Val in 1's place

    UART_print_char(thousand + 0x30);
    UART_print_char(hundred + 0x30);
    UART_print_char(ten + 0x30);
    UART_print_char(one + 0x30);
    UART_print_str(" Hz");
}

/*This function creates the skeleton that forms
 * the UI*/
void DISP_makeUI(void)
{
    uint8_t i;

    /*Clear the screen*/
    UART_esc_code(UART_HOME);
    UART_esc_code(UART_CLEAR);

    /*Print the title*/
    UART_print_str(DISP_TITLE);

    /*Print mode options*/
    UART_esc_code(DISP_OPT_LOC);
    UART_print_str(DISP_OPTIONS);

    /*Print message*/
    UART_esc_code(DISP_MSG_LOC_1);
    UART_print_str(DISP_MSG_1);

    UART_esc_code(DISP_MSG_LOC_2);
    UART_print_str(DISP_MSG_2);


    /*Start printing borders*/
    /*Left side border*/
    UART_esc_code(DISP_LEFT_BORD);

    for(i = 0; i < DISP_INFO_LEN; i++)
        DISP_printDown('|');

    /*Top of information border*/
    UART_esc_code(DISP_INFO_B1);

    for(i = 0; i < DISP_INFO_LEN; i++)
        UART_print_char('_');

    /*Second info border*/
    UART_esc_code(DISP_INFO_B2);

    for(i = 0; i < DISP_INFO_LEN; i++)
        UART_print_char('_');

    /*Third info border*/
    UART_esc_code(DISP_INFO_B3);

    for(i = 0; i < DISP_INFO_LEN; i++)
        UART_print_char('_');

    /*Fourth info border*/
    UART_esc_code(DISP_INFO_B4);

    for(i = 0; i < DISP_INFO_LEN; i++)
        UART_print_char('_');

    /*Top of plot border*/
    UART_esc_code(DISP_PLOT_B1);

    for(i = 0; i < DISP_PLOT_LEN; i++)
        UART_print_char('_');

    /*Second plot border*/
    UART_esc_code(DISP_PLOT_B2);

    for(i = 0; i < DISP_PLOT_LEN; i++)
        UART_print_char('_');

    /*Third plot border*/
    UART_esc_code(DISP_PLOT_B3);

    for(i = 0; i < DISP_PLOT_LEN; i++)
        UART_print_char('_');

    /*Right side border*/
    UART_esc_code(DISP_RIGHT_BORD);

    for(i = 0; i < DISP_RIGHT_HT; i++)
        DISP_printDown('|');

    /*Middle border*/
    UART_esc_code(DISP_MID_BORD);
    DISP_printDown('_');
    UART_esc_code(UART_D1);
    DISP_printDown('|');
    UART_esc_code(UART_D1);
    DISP_printDown('_');
    UART_esc_code(UART_D1);
    DISP_printDown('|');
    UART_esc_code(UART_D1);

    for(i = 0; i < DISP_MID_HT; i++)
        DISP_printDown('|');


    /*Make the boxes where information and plots are located*/
    /*DC information*/
    UART_esc_code(DISP_DC_BOX);
    UART_print_str("DC:");          //Print box title

    UART_esc_code(DISP_VAVG_LABEL);
    UART_print_str("Vavg:");        //Print Vavg label

    UART_esc_code(DISP_DC_LABEL);
    UART_print_str(DISP_BAR_LABEL); //Print Vavg plot label

    /*AC information*/
    UART_esc_code(DISP_AC_BOX);
    UART_print_str("AC:");          //Print box title

    UART_esc_code(DISP_VRMS_LABEL);
    UART_print_str("Vrms:");        //Print Vrms label

    UART_esc_code(DISP_VPP_LABEL);
    UART_print_str("Vp-p:");        //Print Vpp label

    UART_esc_code(DISP_AC_LABEL);
    UART_print_str(DISP_BAR_LABEL); //Print Vrms plot label

    /*Frequency information*/
    UART_esc_code(DISP_FREQ_BOX);
    UART_print_str("Frequency:");   //Print box title
}
