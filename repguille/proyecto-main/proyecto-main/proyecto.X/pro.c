/*
 * File:   pro.c
 * Author: Guille Lam
 *
 * Created on 1 de septiembre de 2021, 05:30 PM
 */
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "lcd.h"
#include "adc.h"
#include "uart.h"
#include "i2c.h"
//CONFIGURATION WORD 1
#pragma config FOSC=INTRC_NOCLKOUT //Oscilador interno sin salida
#pragma config WDTE=OFF           //Reinicio repetitivo del pic
#pragma config PWRTE=OFF           //no espera de 72 ms al iniciar el pic
#pragma config MCLRE=OFF          //El pin MCLR se utiliza como entrada/salida
#pragma config CP=OFF             //Sin protección de código
#pragma config CPD=OFF            //Sin protección de datos
    
#pragma config BOREN=OFF //Sin reinicio cuando el input voltage es inferior a 4V
#pragma config IESO=OFF  //Reinicio sin cambio de reloj de interno a externo
#pragma config FCMEN=OFF //Cambio de reloj externo a interno en caso de fallas
#pragma config LVP=OFF    //Programación en low voltage apagada
    
//CONFIGURATION WORD 2
#pragma config WRT=OFF //Proteccion de autoescritura por el programa desactivada
#pragma config BOR4V=BOR40V //Reinicio abajo de 4V 

#define _XTAL_FREQ 8000000


char s1;  //dato s1
char s2;  //dato s2
float S1;
float S2;
float S3;
char volt[16];        //cadena voltajes
unsigned char pc = 0; //contador +-
volatile char s3 = 0;

void setup(void);


void main(void) {
    setup();
    char volt[16];
    Lcd_Init();
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);  //linea 1
    Lcd_Write_String("S1  S2  S3  S4");
    I2C_Master_Init(100000);
    
  while(1){    
    I2C_Master_Start();
    I2C_Master_Write(0x51);
    PORTB = I2C_Master_Read(0);
    I2C_Master_Stop();
    __delay_ms(50);    //recepcion cont slave1
    I2C_Master_Start();
    I2C_Master_Write(0x61);
    s2 = I2C_Master_Read(0);
    I2C_Master_Stop();
    __delay_ms(50);
    I2C_Master_Start();
    I2C_Master_Write(0x80); //seleccionar el sensor y se escribe
    I2C_Master_Write(0xF3); //read temperature
    I2C_Master_Stop();
    __delay_ms(100);
    I2C_Master_Start();
    I2C_Master_Write(0x81); //para que ahora lea
    PORTA = I2C_Master_Read(0); //read temperature
    I2C_Master_Stop();
    __delay_ms(50);
    s1 = PORTB;
    s3 = PORTA;
    S1= s1*0.0260;
    S2= s2*0.0196;
    S3 = s3-68;
    sprintf(volt, "%.1f  %.1f  %.1f %d\n", S1, S2, S3, pc); //valores para pantalla 2 linea
    enviocadena(volt);                           //envio a pc
    Lcd_Set_Cursor(2,1);                         //linea 2
    Lcd_Write_String(volt); 
    if (PIR1bits.RCIF == 1) {

        switch (RCREG){
            case 43: //+  
                pc++; 
                break;

            case 45: //-
                pc--;
                break;
                }
    }
    __delay_ms(100);
    }return;}

void setup(void){
    //configuracion de pines para potenciometros
    ANSEL = 0;
  
    
    //salidas digitales para controlar LCD
    TRISA = 0x00;
    TRISB = 0x00;
    TRISD = 0x00;
    TRISC = 0x80;
    
    
    //Configurar reloj interno
    OSCCONbits.IRCF = 7;
    OSCCONbits.SCS = 1;  //internal oscillator is used for system clock
    
    //configuracion de interrupciones
    PIE1bits.ADIE = 1;      //enable de la int del ADC
    PIR1bits.ADIF = 0;      //limpiar la interrupcion del ADC
    INTCONbits.GIE = 1;     //habilita las interrupciones globales
    INTCONbits.PEIE = 1;    //periferical interrupts
    PIE1bits.RCIE = 0;      // Interrupcion rx
    PIE1bits.TXIE = 0;      // Interrupcion TX
    
    //limpiar puertos
    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    PORTE = 0x00;
    
//    //configuracion de ADC
    ADC_config(0);
    
    // Configuraciones TX y RX
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    BAUDCTLbits.BRG16 = 1;
    
    SPBRG = 207;
    SPBRGH = 0;
    
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;
    TXSTAbits.TX9 = 0;          // 8 bits
    
    TXSTAbits.TXEN = 1;
    
    PIR1bits.RCIF = 0;  // Bandera rx
    PIR1bits.TXIF = 0;  // bandera tx
    
}


