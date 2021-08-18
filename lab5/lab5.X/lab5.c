/*
 * Archivo:     lab5.c
 * Dispositivo: PIC16F887
 * Autor:       Jose Alejandro Rodriguez Porras
 * Compilador:  XC8 MPLABX V5.40
 * Programa:    lab5 Adafruit
 * Hardware:    2 pushbuttons en PORTB y PC en rcx y rtx
 * Creado:      14 de agosto de 2021, 09:24 PM
 * Ultima modificacion: 14 de agosto de 2021
 */
//CONFIGURATION WORD 1
#include <xc.h>
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
#define _XTAL_FREQ 8000000 //frecuencia de 8 MHz
//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <stdint.h>
#include <stdio.h>
#include "usart.h"
//*****************************************************************************
// Definición de variables
unsigned char cont[];
//*****************************************************************************
#define _XTAL_FREQ 8000000
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void){
    if(RBIF == 1)  {
        //incremento de la variable con antirebote
        if (PORTBbits.RB0 == 0) {
            PORTA++; 
            sprintf(cont, "%d", PORTA);
            enviocadena(cont);
        }
        //decremento de la variable con antirebote
        if (PORTBbits.RB1 == 0) {
            PORTA--; 
            sprintf(cont, "%d", PORTA);
            enviocadena(cont);
        }
        //se baja la bandera de interrupcion por cambio
        INTCONbits.RBIF = 0;
    }
}
//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    while(1){if (PIR1bits.RCIF == 1) {PORTD = RCREG;}}
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    //configuracion
    //reloj
    
    OSCCONbits.IRCF = 7; //8MHz
    OSCCONbits.SCS = 1; //reloj interno
    //configuracion in out
    ANSELH = 0; //Pines digitales
    ANSEL = 0;
    TRISA = 0; 
    TRISB = 3;
    TRISD = 0;
    OPTION_REGbits.nRBPU = 0; //pullups habilitados
    WPUBbits.WPUB0 = 1;       //pullups RB0 y RB1 
    WPUBbits.WPUB1 = 1;
    PORTA = 0;         //se limpian los puertos
    PORTB = 0;
    PORTD = 0;
    //configuracion interrupt on change b
    IOCBbits.IOCB0 = 1; //Se habilita la interrupcion por cambio en RB0 y RB1
    IOCBbits.IOCB1 = 1;
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
    //configuracion interrupciones
    INTCONbits.GIE  = 1; //se habilitan las interrupciones globales
    INTCONbits.RBIE = 1; //interrupcion on change habilitada 
    INTCONbits.PEIE = 1; //se habilitan las interrupciones de los perifericos
    sprintf(cont, "%d", PORTA);
    enviocadena(cont);
}