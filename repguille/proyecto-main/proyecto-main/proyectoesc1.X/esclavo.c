/*
 * File:   esclavo.c
 * Author: Guille Lam
 *
 * Created on 2 de septiembre de 2021, 12:27 AM
 */


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
#include "i2c.h"
#include "adc.h"

//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t z;

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
            PORTD = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = PORTB;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
   if (ADIF == 1){
       if (ADCON0bits.CHS == 0){
//        ADCON0bits.CHS = 1;     //se actualiza la variable con valor del adc
        PORTB = ADRESH;
        __delay_us(20);   //delay de 20 ms
        PIR1bits.ADIF = 0;//se baja bandera interrupcion adc
//        ADCON0bits.GO = 1;//inicio de la siguiente conversion
        }
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
    while(1){
     if(ADCON0bits.GO == 0){
        __delay_ms(100);
        ADCON0bits.GO = 1;}
    }
    return;
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
    ANSELbits.ANS0  = 1;//RA0 como pines analogicos
    TRISA = 1;         //RA0 y RA1 como inputs
    TRISB = 0;
    TRISD = 0;
    PORTA = 0;         //se limpian los puertos
    PORTB = 0;
    PORTD = 0;
    //adc
    PIE1bits.ADIE = 1;      //enable de la int del ADC
    PIR1bits.ADIF = 0;      //limpiar la interrupcion del ADC
    INTCONbits.GIE = 1;     //habilita las interrupciones globales
    INTCONbits.PEIE = 1;    //periferical interrupts
    ADC_config(0); //configuracion adc (usando funcion de libreria)
    __delay_us(20);
    I2C_Slave_Init(0x50);  
    
    
}
