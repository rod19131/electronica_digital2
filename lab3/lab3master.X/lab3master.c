//*****************************************************************************
/*
 * File:   lab3master.c
 * Author: Alejandro Rodríguez
 * comunicación SPI 
 * Código Maestro
 * Created on 4 de agosto de 2021, 01:00 AM
 */
//*****************************************************************************
//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
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
//*****************************************************************************
// Definición e importación de librerías
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include "SPI.h"
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
unsigned char dummydata = 0;
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);

//*****************************************************************************
// Código Principal
//*****************************************************************************
void main(void) {
    setup();
    //*************************************************************************
    // Loop infinito
    //*************************************************************************
    while(1){
        if (dummydata == 1){
            PORTCbits.RC2 = 0;       //Slave Select
            __delay_ms(1);

            spiWrite(dummydata);
            PORTB = spiRead();

            __delay_ms(1);

     //       spiWrite(dummydata);
     //       PORTD = spiRead();
     //       __delay_ms(1);
            PORTCbits.RC2 = 1;       //Slave Deselect 

            __delay_ms(100);
            dummydata = 0;
       }
        else {
            PORTCbits.RC2 = 0;       //Slave Select
            __delay_ms(1);

            spiWrite(dummydata);
            PORTD = spiRead();

            __delay_ms(1);

     //       spiWrite(dummydata);
     //       PORTD = spiRead();
     //       __delay_ms(1);
            PORTCbits.RC2 = 1;       //Slave Deselect 

            __delay_ms(100);
            dummydata = 1;
        }
    }
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
    TRISB = 0;
    TRISC = 144;
    TRISD = 0;
    PORTA = 0;         //se limpian los puertos
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTCbits.RC2 = 1;
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);

}