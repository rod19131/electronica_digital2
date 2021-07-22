/*
 * Archivo:     lab1otro.c
 * Dispositivo: PIC16F887
 * Autor:       Jose Alejandro Rodriguez Porras
 * Compilador:  XC8 MPLABX V5.40
 * Programa:    Lab1 Interrupciones y librerias
 * Hardware:    LEDs en puerto C, Displays en puerto D, Transistores,PBs en 
 * puerto B y Potenciometro en puerto A
 * Creado:      20 de julio de 2021, 10:52 PM
 * Ultima modificacion: 20 de julio de 2021
 */
#include <xc.h>
#include "adc_c.h"    //libreria configuracion adc
#include "adc_canal.h"//libreria canal adc
#include "tabla7seg.h"//libreria tabla7seg
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
#define _XTAL_FREQ 1000000 //frecuencia de 8 MHz
//-------------------------------variables--------------------------------------
unsigned char c = 0;      //valor adc    
unsigned char dispvar = 0;//multiplexado de displays

void __interrupt() isr(void){    // only process timer-triggered interrupts
    //interrupcion por cambio (boton)
    if(RBIF == 1)  {
        //subir contador con antirebote
        if (PORTBbits.RB0 == 0) {
            PORTC++;
        }
        //bajar contador con antirebote
        if (PORTBbits.RB1 == 0) {
            PORTC--;
        }
        //se baja la bandera de interrupcion por cambio
        INTCONbits.RBIF = 0;
    }
    if (ADIF == 1) {
        //conversion adc
        //alarma
        if (c == PORTC){
                  PORTBbits.RB4 = 1;
              }
        else {
                  PORTBbits.RB4 = 0;
        }
        c = adc_canal(0);     //se actualiza la variable con valor del adc
        __delay_us(20);   //delay de 20 ms
        PIR1bits.ADIF = 0;//se baja bandera interrupcion adc
        ADCON0bits.GO = 1;//inicio de la siguiente conversion
    }
    //interrupcion del tmr0
     if (T0IF == 1) {
        TMR0 = 0;//reset tmr0
        //multiplexacion de los displays (usando funcion de libreria tabla7seg)
        switch(dispvar) {
            case 0: 
                PORTBbits.RB2 = 0;
                PORTD = tabla7seg(c & 0x0F);//1a posicion hexa en disp1 )
                PORTBbits.RB3 = 1;
                dispvar = 1; 
                break;
            case 1:
                PORTBbits.RB3 = 0;
                PORTD = tabla7seg((c & 0xF0) >> 4);//2a posicion hexa en disp2
                PORTBbits.RB2 = 1;
                dispvar = 0;      
                break;
        }
    INTCONbits.T0IF = 0; //se baja la bandera de interrupcion del tmr0
     }    
  
}
    
void main(void) {
    //configuracion
    //reloj
    OSCCONbits.IRCF = 4; //1MHz
    OSCCONbits.SCS = 1; //reloj interno
    //configuracion in out
    ANSELH = 0; //Pines digitales
    ANSELbits.ANS0  = 1;//RA0 como pines analogicos
    TRISA  = 0;
    TRISB  = 3;//input RB0, RB1 y todos los demas pines como out
    TRISC  = 0;
    TRISD  = 0;
    OPTION_REGbits.nRBPU = 0; //pullups habilitados
    WPUBbits.WPUB0 = 1;       //pullups RB0, RB1
    WPUBbits.WPUB1 = 1;
    PORTA  = 0;               //se limpian los puertos
    PORTB  = 0;
    PORTC  = 0;
    PORTD  = 0;
    //interrupt on change b
    IOCBbits.IOCB0 = 1; //interrupcion por cambio habilitada en RB0, RB1, RB2
    IOCBbits.IOCB1 = 1;
    //tmr0
    OPTION_REGbits.T0CS = 0; //reloj interno (low to high)
    OPTION_REGbits.PSA  = 0; //prescaler 
    OPTION_REGbits.PS = 0; //1; 1:1
    TMR0 = 0;
    INTCONbits.T0IF = 0; //interrupcion overflow tmr0 habilitada
    //adc
    adc_c(); //configuracion adc (usando funcion de libreria)
    //configuracion interrupciones
    INTCONbits.GIE  = 1; //se habilitan las interrupciones globales
    INTCONbits.T0IE = 1; //interrupcion overflow tmr0 habilitada
    INTCONbits.RBIE = 1; //interrupcion on change habilitada
    INTCONbits.PEIE = 1; //se habilitan las interrupciones de los perifericos
    PIE1bits.ADIE = 1;   //se habilitan las interrupciones por adc
    ADCON0bits.GO = 1;  //se comienza la conversion adc
    while (1){}
     }
