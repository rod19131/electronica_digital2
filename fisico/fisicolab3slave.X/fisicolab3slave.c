//*****************************************************************************
/*
 * File:   lab3slave.c
 * Author: Alejandro Rodriguez
 * comunicación SPI 
 * Código Esclavo
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
#include <stdio.h>
#include "adc_c.h"
#include "adc_canal.h"
#include "usart.h"
#include "SPI.h"

//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
unsigned char conver = 0;
unsigned char dummydata = 0;
unsigned char spiflag = 0;
//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);
//*****************************************************************************
// Código de Interrupción 
//*****************************************************************************
void __interrupt() isr(void){
   if(SSPIF == 1){
        dummydata = spiRead();
        if (dummydata == 0){
            spiWrite(PORTB);}
        else{
            spiWrite(PORTD);}
        SSPIF = 0;
    }
   if (ADIF == 1){
       if (ADCON0bits.CHS == 0){
       PORTB = adc_canal(1);     //se actualiza la variable con valor del adc
        __delay_us(20);   //delay de 20 ms
        PIR1bits.ADIF = 0;//se baja bandera interrupcion adc
        ADCON0bits.GO = 1;//inicio de la siguiente conversion
        }
       else {
        PORTD = adc_canal(0);     //se actualiza la variable con valor del adc
        __delay_us(20);   //delay de 20 ms
        PIR1bits.ADIF = 0;//se baja bandera interrupcion adc
        ADCON0bits.GO = 1;//inicio de la siguiente conversion
        }
       }
    }
//*****************************************************************************
// Código Principal
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
    ANSELbits.ANS0  = 1;//RA0 como pines analogicos
    ANSELbits.ANS1  = 1;//RA1 como pines analogicos
    TRISA = 35;         //RA0 y RA1 como inputs
    TRISB = 0;
    TRISCbits.TRISC2 = 0;
    TRISD = 0;
    PORTA = 0;         //se limpian los puertos
    PORTB = 0;
    PORTD = 0;
    //adc
    adc_c(); //configuracion adc (usando funcion de libreria)
    __delay_us(20);
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Se habilitan las interrupciones perifericos
    PIR1bits.SSPIF = 0;         // Borramos bandera interrupción MSSP
    PIE1bits.SSPIE = 1;         // Habilitamos interrupción MSSP
    TRISAbits.TRISA5 = 1;       // Slave Select
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    PIE1bits.ADIE = 1;   //se habilitan las interrupciones por adc
    ADCON0bits.GO = 1;  //se comienza la conversion adc
}