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
#include "usart.h"
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
#define RS RC0         //asignacion botones
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
unsigned char s1 = 0; //dato s1
unsigned char s2 = 0; //dato s2
float S1, S2 = 0;     //decimal s1 y s2
char volt[16];        //cadena voltajes
unsigned char pc = 0; //contador +-
char pcchar = 0;
//______________________________funciones_______________________________________
float mapear(unsigned char adresval){
    return (adresval-0)*(5.00-0)/(255-0.0)+0;}

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
    TRISC = 128;
    TRISD = 0;
    PORTA = 0;         //se limpian los puertos
    PORTC = 0;
    PORTD = 0;
    //adc
    adc_c(); //configuracion adc (usando funcion de libreria)
    Lcd_Init();
    //TX y RX
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    
    BAUDCTLbits.BRG16 = 1;
    
    SPBRG = 207;
    SPBRGH = 0;
    
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.CREN = 1;
    TXSTAbits.TXEN = 1;
//    TXSTAbits.TX9 = 1;
    //configuracion interrupciones
    INTCONbits.GIE  = 1; //se habilitan las interrupciones globales
    INTCONbits.RBIE = 1; //interrupcion on change habilitada
    INTCONbits.PEIE = 1; //se habilitan las interrupciones de los perifericos
//    PIE1bits.RCIE = 1;
//    PIR1bits.RCIF = 0;
//    PIR1bits.TXIF = 0;
    PIE1bits.ADIE = 1;   //se habilitan las interrupciones por adc
    ADCON0bits.GO = 1;  //se comienza la conversion adc
  while(1)    
  {if (ADCON0bits.GO == 0) {
        s2 = adc_canal(0);     //se actualiza la variable con valor del adc
        __delay_us(20);   //delay de 20 ms
        PIR1bits.ADIF = 0;//se baja bandera interrupcion adc
        ADCON0bits.GO = 1;//inicio de la siguiente conversion
    }
    Lcd_Set_Cursor(1,1);  //linea 1
    Lcd_Write_String("S1  S1  S3");
    S1 = mapear(s1);      //mapeo de valores de 0 a 5 V
    S2 = mapear(s2);
    sprintf(volt, "%.2f  %.2f  %d\n", S1, S2, pc); //valores para pantalla 2 linea
    enviocadena(volt);                           //envio a pc
    Lcd_Set_Cursor(2,1);                         //linea 2
    Lcd_Write_String(volt);                      
    if (ADCON0bits.GO == 0) {
        s1 = adc_canal(1);     //se actualiza la variable con valor del adc
        __delay_us(20);   //delay de 20 ms
        PIR1bits.ADIF = 0;//se baja bandera interrupcion adc
        ADCON0bits.GO = 1;//inicio de la siguiente conversion
    }
    if (PIR1bits.RCIF == 1) {
//        pcchar = RCREG;
//        if (RCREG == '+'){
//            pc++;
//        }
//        else if (RCREG == '-'){
//            pc--;
//        }
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
    }
}

