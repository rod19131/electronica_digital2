/*
 * File:   lab2.c
 * Author: Alejandro Rodríguez
 *
 * Created on 28 de julio de 2021, 01:12 AM
 */
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include "LCD8bits.h"
#include "adc_c.h"
#include "adc_canal.h"
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
#define RS RC0
#define EN RC1
#define D0 RD0
#define D1 RD1
#define D2 RD2
#define D3 RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7
#define _XTAL_FREQ 8000000

//______________________________variables_______________________________________
unsigned char s1 = 0;
unsigned char s2 = 0;
unsigned int a = 0;
float S1, S2 = 0;
char volt[16];
//______________________________funciones_______________________________________
float mapear(unsigned char adresval){
    return (adresval-0)*(5.00-0)/(255-0.0)+0;}
//void __interrupt() isr(void){    // only process timer-triggered interrupts
//    //adc
//    if (ADIF == 1) {
//        //conversion adc
//        switch (ADCON0bits.CHS){
//            case 0:
//                s1 = adc_canal(0);
//                break;
//                
//            case 1:
//                s2 = adc_canal(1);
//                break;   
//        }
//        __delay_us(20);   //delay de 20 ms
//        PIR1bits.ADIF = 0;//se baja bandera interrupcion adc
//        ADCON0bits.GO = 1;//inicio de la siguiente conversion
//    }  
//}

void main(void) {
    //configuracion
    //reloj
    OSCCONbits.IRCF = 7; //8MHz
    OSCCONbits.SCS = 1; //reloj interno
    //configuracion in out
    ANSELH = 0; //Pines digitales
    ANSELbits.ANS0  = 1;//RA0 como pines analogicos
    ANSELbits.ANS1  = 1;//RA1 como pines analogicos
    TRISA = 3;         //RA0 y RA1 como inputs
    TRISC = 0;
    TRISD = 0;
    PORTA = 0;               //se limpian los puertos
    PORTC = 0;
    PORTD = 0;
    //adc
    adc_c(); //configuracion adc (usando funcion de libreria)
    Lcd_Init();
    //configuracion interrupciones
    INTCONbits.GIE  = 1; //se habilitan las interrupciones globales
    INTCONbits.RBIE = 1; //interrupcion on change habilitada
    INTCONbits.PEIE = 1; //se habilitan las interrupciones de los perifericos
    PIE1bits.ADIE = 1;   //se habilitan las interrupciones por adc
    ADCON0bits.GO = 1;  //se comienza la conversion adc
  while(1)
      
  { if (ADCON0bits.GO == 0) {
        s2 = adc_canal(0);     //se actualiza la variable con valor del adc
        __delay_us(20);   //delay de 20 ms
        PIR1bits.ADIF = 0;//se baja bandera interrupcion adc
        ADCON0bits.GO = 1;//inicio de la siguiente conversion
    }
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("S1  S1  S3");
    S1 = mapear(s1);
    S2 = mapear(s2);
    sprintf(volt, "%.2f  %.2f  %d" , S1, S2, s1);
    Lcd_Set_Cursor(2,1);
    Lcd_Write_String(volt);
    if (ADCON0bits.GO == 0) {
        s1 = adc_canal(1);     //se actualiza la variable con valor del adc
        __delay_us(20);   //delay de 20 ms
        PIR1bits.ADIF = 0;//se baja bandera interrupcion adc
        ADCON0bits.GO = 1;//inicio de la siguiente conversion
    }
    __delay_ms(100);
  }
}

