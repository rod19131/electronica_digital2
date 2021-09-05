/*
 * File:   lab2.c
 * Author: Guille Lam
 *
 * Created on 27 de julio de 2021, 05:50 PM
 */

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT    // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF       // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V     // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)


#define _XTAL_FREQ 4000000 
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "lcd.h"
#include "adc.h"
#include "uart.h"


//Variables 
uint8_t adc0, adc1;
uint8_t num = 0;
float num1,num2;
char BUFFER[20];
char lectura;

//Prototipos 
void config(void);
void putch(char data);

//Interrupciones
void __interrupt() isr(void){ 
    if (RCIF == 1) {
        RCIF = 0;
        lectura = LEER();
        if (lectura == '+'){
            num = num + 1;}
        else if (lectura == '-'){
            num = num - 1;}
    
    else{ 
        NULL;}  
    }}
//Main
void main(void) {
    config();
    baud();
    config_tx();
    config_rx();
    configLCD();
    limpiar();
    while(1){
        limpiar();
        cursor(1,1);
        ESCLCD("V1   V2   CONT"); //LCD
        
        adc0 = configADC(0);
        adc1 = configADC(1);
           
        num1 = adc0*0.0196;
        num2 = adc1*0.0196;
        sprintf(BUFFER, "%.2f  %.2f  %d" , num1,num2, num);
        //sprintf(BUFFER,"%d", V1);
      
        cursor(2,1);
        ESCLCD(BUFFER);   //LCD
        
        ESCString(BUFFER);
        ESC(3);
        ESC(10);
        __delay_ms(250);
    }
}

//Funciones
void config(void) {
    ANSEL = 0;
    ANSELH = 0;
    PORTD = 0;
    PORTE = 0;
    PORTC = 0;
    TRISA = 0;
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;
    TRISD = 0;
    TRISE = 0;

    INTCONbits.PEIE = 1;
    PIE1bits.RCIE = 1;
    PIR1bits.RCIF = 0;
    INTCONbits.GIE = 1;
    PIE1bits.TXIE = 0;
    PIR1bits.RCIF = 1;  
    PIR1bits.TXIF = 1;  
}

void putch(char data){      
    while(TXIF == 0);
    TXREG = data; 
    return;
}



