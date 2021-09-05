/*
 * File:   lab2.c
 * Author: Guille Lam
 *
 * Created on 27 de julio de 2021, 05:50 PM
 */

#include <xc.h> 
#include <stdint.h>  
#include "adc.h"

#define _XTAL_FREQ 4000000

uint8_t configADC(uint8_t ch) {
    ADCON0bits.ADCS = 1;  
    ADCON1 = 0;
    
    switch (ch) {   
        case 0:   // pin A0 
            TRISAbits.TRISA0 = 1;
            ANSELbits.ANS0 = 1;
            ADCON0bits.CHS = 0;
            break;
        case 1: // pin A1 
            TRISAbits.TRISA1=1; 
            ANSELbits.ANS1=1; 
            ADCON0bits.CHS=1; 
            break;
        case 2: // pin A2 
            TRISAbits.TRISA2=1; 
            ANSELbits.ANS2=1; 
            ADCON0bits.CHS=2; 
            break;
        case 3: // pin A3 
            TRISAbits.TRISA3=1; 
            ANSELbits.ANS3=1; 
            ADCON0bits.CHS=3; 
            break;
        case 4: // pin RA5
            TRISAbits.TRISA5=1; 
            ANSELbits.ANS4=1; 
            ADCON0bits.CHS=4; 
            break;
        case 5: // pin E0 
            TRISEbits.TRISE0=1; 
            ANSELbits.ANS5=1; 
            ADCON0bits.CHS=5; 
            break;
        case 6: // pin E1 
            TRISEbits.TRISE1=1; 
            ANSELbits.ANS6=1; 
            ADCON0bits.CHS=6; 
            break;
        case 7: // pin E2 
            TRISEbits.TRISE2=1; 
            ANSELbits.ANS7=1; 
            ADCON0bits.CHS=7; 
            break;
        case 8: // pin B2 
            TRISBbits.TRISB2=1; 
            ANSELHbits.ANS8=1; 
            ADCON0bits.CHS=8; 
            break;
        case 9: // pin B3 
            TRISBbits.TRISB3=1; 
            ANSELHbits.ANS9=1; 
            ADCON0bits.CHS=9; 
            break;
        case 10: // pin B1 
            TRISBbits.TRISB1=1; 
            ANSELHbits.ANS10=1; 
            ADCON0bits.CHS=10; 
            break;
        case 11: // pin B4 
            TRISBbits.TRISB4=1; 
            ANSELHbits.ANS11=1; 
            ADCON0bits.CHS=11; 
            break;
        case 12: // pin B0 
            TRISBbits.TRISB0=1; 
            ANSELHbits.ANS12=1; 
            ADCON0bits.CHS=12; 
            break;
        case 13: // pin B5 
            TRISBbits.TRISB5=1; 
            ANSELHbits.ANS13=1; 
            ADCON0bits.CHS=13; 
            break;
        default: 
            return 0;
    }
 
    ADCON0bits.ADON = 1;    
    __delay_ms(0.25);
    ADCON0bits.GO = 1;
    
    resul:
    if (ADCON0bits.GO_DONE == 1){
        goto resul; 
    }     
    else{
        ADCON0bits.ADON = 0;
        return ADRESH;  
    }
}