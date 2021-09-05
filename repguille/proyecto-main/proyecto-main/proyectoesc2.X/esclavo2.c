/*
 * File:   esclavo2.c
 * Author: Guille Lam
 *
 * Created on 2 de septiembre de 2021, 12:36 AM
 */


#include <xc.h>
#pragma config FOSC=INTRC_NOCLKOUT //Oscilador interno sin salida
#pragma config WDTE=OFF           //Reinicio repetitivo del pic
#pragma config PWRTE=OFF           //no espera de 72 ms al iniciar el pic
#pragma config MCLRE=OFF          //El pin MCLR se utiliza como entrada/salida
#pragma config CP=OFF             //Sin protecci�n de c�digo
#pragma config CPD=OFF            //Sin protecci�n de datos
    
#pragma config BOREN=OFF //Sin reinicio cuando el input voltage es inferior a 4V
#pragma config IESO=OFF  //Reinicio sin cambio de reloj de interno a externo
#pragma config FCMEN=OFF //Cambio de reloj externo a interno en caso de fallas
#pragma config LVP=OFF    //Programaci�n en low voltage apagada
    
//CONFIGURATION WORD 2
#pragma config WRT=OFF //Proteccion de autoescritura por el programa desactivada
#pragma config BOR4V=BOR40V //Reinicio abajo de 4V 
#define _XTAL_FREQ 8000000 //frecuencia de 8 MHz
//*****************************************************************************
// Definici�n e importaci�n de librer�as
//*****************************************************************************
#include <stdint.h>
#include "I2C.h"
//*****************************************************************************
// Definici�n de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t z;
//*****************************************************************************
// Definici�n de funciones para que se puedan colocar despu�s del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
//*****************************************************************************
// C�digo de Interrupci�n 
//*****************************************************************************
void __interrupt() isr(void){
    if(RBIF == 1)  {
        //incremento de la variable con antirebote
        if (PORTBbits.RB0 == 0) {
            PORTA++; 
        }
        //decremento de la variable con antirebote
        if (PORTBbits.RB1 == 0) {
            PORTA--; 
        }
        //se baja la bandera de interrupcion por cambio
        INTCONbits.RBIF = 0;
    }
   if(PIR1bits.SSPIF == 1){ 
        SSPCONbits.CKP = 0;
       
        if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL)){
            z = SSPBUF;                 // Read the previous value to clear the buffer
            SSPCONbits.SSPOV = 0;       // Clear the overflow flag
            SSPCONbits.WCOL = 0;        // Clear the collision bit
            SSPCONbits.CKP = 1;         // Enables SCL (Clock)
        }

        if(!SSPSTATbits.D_nA && !SSPSTATbits.R_nW) {
            //__delay_us(7);
            z = SSPBUF;                 // Lectura del SSBUF para limpiar el buffer y la bandera BF
            //__delay_us(2);
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupci�n recepci�n/transmisi�n SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepci�n se complete
            PORTD = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepci�n
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = PORTA;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
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
    while(1){}
}
//*****************************************************************************
// Funci�n de Inicializaci�n
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
    //configuracion interrupciones
    INTCONbits.GIE  = 1; //se habilitan las interrupciones globales
    INTCONbits.RBIE = 1; //interrupcion on change habilitada
    I2C_Slave_Init(0x60);  
}