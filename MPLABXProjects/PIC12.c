
/*---------info-------------
 * PIC12F683 Code for Arduino data logger
 * ---add github link---
 * Created: 10/01/2015 by Salvatore R Curasi
 * Last modified: 7/8/2019
 * See read_me.txt for additional information
 * ----------------------------
 * 
 * --------Directions----------
 * BASIC FUNCTIONALITY:
 * This code controls the data logging interval and allows the data logger
 * to conserve power.
 
 SETUP:
 * Line 51 controls the data logging interval. The value of "unsigned int 
 * interval" can be calculated as "desired time in minutes"*99.776 rounded 
 * to the nearest whole number  
 */

#include <xc.h>

#pragma config FOSC = INTOSCIO
#pragma config WDTE = OFF
#pragma config PWRTE = ON  
#pragma config MCLRE = OFF
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config IESO = OFF

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 4000000
#endif

#define Led1 GPIObits.GP4
#define Sht GPIObits. GP2
#define Sig GPIObits. GP3

void main()
{
    ANSEL = 0x00; 
    ADCON0 = 0x00; 
    CMCON0 = 0x07; 
    VRCON = 0x00; 
    TRISIO = 0x08; 
    GPIO = 0x00; 
    OPTION_REG = 000;
    WDTCON = 1010;

    unsigned int interval = 100;//sets the delay 
    //"desired time in minutes"*99.776 rounded to the nearest whole number
    unsigned int ct;
    unsigned int i;
    unsigned int timeout = 500;//max time Arduino will run per cycle

    Led1 = 1;
    __delay_ms(1000);
    Led1 = 0;

    SWDTEN = 1;

    while(1)
    {
        ct++;
        if (ct >= interval){
            SWDTEN = 0;
            ct = 0;
            Sht = 1;
            __delay_ms(100);
            for(i=0; i<timeout; i++)
            {
                if (Sig == 1)
                {
                   break;
                }
                 __delay_ms(10);
            }
            Sht = 0;
            SWDTEN = 1;
            }
        SLEEP();
        }
}

