/*
 * Archivo:     proyectovacas.c
 * Dispositivo: PIC16F887
 * Autor:       Jose Alejandro Rodriguez Porras
 * Compilador:  XC8 MPLABX V5.40
 * Programa:    Proyecto vacaciones
 * Hardware:    2 pots en puerto A y 2 servos en CCP1 y CCP2
 * Creado:      14 de junio de 2021, 05:09 PM
 * Ultima modificacion: 26 de abril de 2021
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
#include "I2C.h"
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t z;
unsigned char motores, M, dummydata = 0;
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void){
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
            PIR1bits.SSPIF = 0;         // Limpia bandera de interrupción recepción/transmisión SSP
            SSPCONbits.CKP = 1;         // Habilita entrada de pulsos de reloj SCL
            while(!SSPSTATbits.BF);     // Esperar a que la recepción se complete
            M = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            if (M == 0){
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
            }
            if (M == 1){
                if (PORTBbits.RB2 == 1 && PORTBbits.RB3 == 1){
                PORTD = 0;
                }
                else if (PORTBbits.RB2 == 0 && PORTBbits.RB3 == 0){
                    PORTD = 0b00101101;
                }
                else if (PORTBbits.RB2 == 1 && PORTBbits.RB3 == 0){
                    PORTD = 0b00000101;
                }
                else if (PORTBbits.RB2 == 0 && PORTBbits.RB3 == 1){
                    PORTD = 0b00101000;
                }
            }
            SSPBUF = PORTD;
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
    TRISB = 15;
    TRISD = 0;
    OPTION_REGbits.nRBPU = 0; //pullups habilitados
    WPUBbits.WPUB0 = 1;       //pullups RB0 y RB1 
    WPUBbits.WPUB1 = 1;
    PORTA = 0;         //se limpian los puertos
    PORTB = 0;
    PORTD = 0;
    I2C_Slave_Init(0x60);  
}
