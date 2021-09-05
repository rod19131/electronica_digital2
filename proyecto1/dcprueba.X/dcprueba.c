/*
 * Archivo:     lab0.c
 * Dispositivo: PIC16F887
 * Autor:       Jose Alejandro Rodriguez Porras
 * Compilador:  XC8 MPLABX V5.40
 * Programa:    Lab0 juego carrera
 * Hardware:    LEDs en puertos A, B y C, PB en puerto B y 7seg disp en puerto D
 * Creado:      14 de julio de 2021, 05:09 PM
 * Ultima modificacion: 17 de julio de 2021
 */

#include <xc.h>
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

//variables
    
void main(void) {
    //configuracion
    //reloj
    OSCCONbits.IRCF = 1; //125KHz
    OSCCONbits.SCS = 1; //reloj interno
    //configuracion in out
    ANSELH = 0; //Pines digitales
    ANSEL  = 0;
    TRISA  = 0;
    TRISB  = 3;//input RB0, RB1, RB2 y todos los demas pines como out
    TRISC  = 0;
    TRISD  = 0;
//    OPTION_REGbits.nRBPU = 0; //pullups habilitados
//    WPUBbits.WPUB0 = 1;       //pullups RB0, RB1 y RB2 
//    WPUBbits.WPUB1 = 1;
    PORTA  = 0;               //se limpian los puertos
    PORTB  = 0;
    PORTC  = 0;
    PORTD  = 0;
    while (1){
        if (PORTBbits.RB0 == 1 && PORTBbits.RB1 == 1){
            PORTD = 0;
        }
        else if (PORTBbits.RB0 == 0 && PORTBbits.RB1 == 0){
            PORTD = 0b00101101;
        }
        else if (PORTBbits.RB0 == 1 && PORTBbits.RB1 == 0){
            PORTD = 0b00000101;
        }
        else if (PORTBbits.RB0 == 0 && PORTBbits.RB1 == 1){
            PORTD = 0b00101000;
        }
              }}
