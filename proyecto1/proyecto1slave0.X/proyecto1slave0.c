/*
 * Archivo:     proyecto1slave0.c
 * Dispositivo: PIC16F887
 * Autor:       Jose Alejandro Rodriguez Porras
 * Compilador:  XC8 MPLABX V5.40
 * Programa:    esclavo0 sensor de iluminacion y control de servo
 * Hardware:    servo en PORTC, leds en PORTD y fotoresistencia en RA0
 * Creado:      1 de septiembre de 2021, 05:09 PM
 * Ultima modificacion: 8 de septiembre de 2021
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
#include "adc_c.h"
#include "adc_canal.h"
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t z;
unsigned char luzadc, dummydata, luz, mapping, dummy =  0;
unsigned char mapear(unsigned char adresval){
    return (adresval-0)*(9-0)/(255-0)+0;}
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
            mapping = SSPBUF;             // Guardar en el PORTD el valor del buffer de recepción
            __delay_us(250);
            
        }else if(!SSPSTATbits.D_nA && SSPSTATbits.R_nW){
            z = SSPBUF;
            BF = 0;
            SSPBUF = luz;
            SSPCONbits.CKP = 1;
            __delay_us(250);
            while(SSPSTATbits.BF);
        }
       
        PIR1bits.SSPIF = 0;    
    }
   if (ADIF == 1){
       //lectura de fotoresistencia para conocer iluminacion/mapeo de servo a norte
       if (ADCON0bits.CHS == 0){
            luzadc = adc_canal(0);     //se actualiza la variable con valor del adc
            //CCPR1L = adc_canal(0);
            luz = mapear(luzadc);
            ADCON0bits.CHS = 0;
            if (luz <= 3){PORTD = 7;}
            else if (luz <= 6){PORTD = 6;}
            else {PORTD = 0;}  
            __delay_us(20);   //delay de 20 ms
            PIR1bits.ADIF = 0;//se baja bandera interrupcion adc
            ADCON0bits.GO = 1;//inicio de la siguiente conversion
            //mapeo del servo para indicar donde esta el norte
            CCPR1L = (mapping >> 1) + 124;
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
    ANSEL = 7;
    ANSELH = 0; //Pines digitales
    ANSELbits.ANS0  = 1;//RA0 como pines analogicos
    TRISA = 7;         //RA0 y RA1 como inputs
    TRISB = 0;
    TRISD = 0;
    PORTA = 0;         //se limpian los puertos
    PORTB = 0;
    PORTD = 0;
    //adc
    //configuracion pwm
    //ccp1
    TRISCbits.TRISC2 = 1;      //CCP1 como entrada;
    PR2 = 250;                 //valor para que el periodo pwm sea 2 ms 
    CCP1CONbits.P1M = 0;       //config pwm
    CCP1CONbits.CCP1M = 0b1100;
    CCPR1L = 0x0f;             //ciclo de trabajo inicial
    CCP1CONbits.DC1B = 0;
    //ccp2
    TRISCbits.TRISC1 = 1;      //CCP2 como entrada;
    CCP2CONbits.CCP2M = 0b1100;//config pwm
    CCPR2L = 0x0f;             //ciclo de trabajo inicial
    CCP2CONbits.DC2B1 = 0;
    //configuracion tmr2
    PIR1bits.TMR2IF = 0; //se apaga la bandera de interrupcion del tmr2
    T2CONbits.T2CKPS = 0b11;//prescaler 1:16
    T2CONbits.TMR2ON = 1;//se enciende el tmr2
    while(PIR1bits.TMR2IF == 0);//esperar un ciclo de tmr2
    PIR1bits.TMR2IF = 0;
    TRISCbits.TRISC1 = 0;//out pwm2
    TRISCbits.TRISC2 = 0;//out pwm1
    adc_c(); //configuracion adc (usando funcion de libreria)
    __delay_us(20);
    I2C_Slave_Init(0x50);  
    PIE1bits.ADIE = 1;   //se habilitan las interrupciones por adc
    ADCON0bits.GO = 1;  //se comienza la conversion adc
}