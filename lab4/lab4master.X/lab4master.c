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
#include <stdio.h>
#include "I2C.h" 
#include "LCD8bits.h"
//*****************************************************************************
// Definición de variables
//*****************************************************************************
#define RS RC0         //asignacion lcd
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
//variables
unsigned int senms = 0;    //valores sensor i2c buf
unsigned int sensorval = 0;//valores sensor i2c
double t = 0;              //conversion temperatura
unsigned char s1, s2 = 0;  //valores esclavos
char volt[16];        //cadena LCD valores

//*****************************************************************************
// Definición de funciones para que se puedan colocar después del main de lo 
// contrario hay que colocarlos todas las funciones antes del main
//*****************************************************************************
void setup(void);

//*****************************************************************************
// Main
//*****************************************************************************
void main(void) {
    setup();
    while(1){
        //recepcion adc slave0
        I2C_Master_Start();
        I2C_Master_Write(0x51);
        s1 = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(50);
        //recepcion cont slave1
        I2C_Master_Start();
        I2C_Master_Write(0x61);
        s2 = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(50);
        
        //recepcion sensor i2c temp
        I2C_Master_Start();
        I2C_Master_Write(0x80);
        I2C_Master_Write(0xF3);
        I2C_Master_Stop();
        __delay_ms(100);
        
        I2C_Master_Start();
        I2C_Master_Write(0x81);
        senms = I2C_Master_Read(0);
        sensorval = I2C_Master_Read(0);  
        I2C_Master_Stop();
        __delay_ms(50);
        
        //conversion temperatura
        sensorval = senms<<8;
        sensorval += senms;
        sensorval &= ~0b11;
        t = -46.85 +(175.72*sensorval/65536);
        //escribir valores en LCD
        sprintf(volt, "%d   %d    %.0f  ", s1, s2, t); //valores para pantalla 2 linea
        Lcd_Set_Cursor(2,1);                         //linea 2
        Lcd_Write_String(volt);
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
    ANSEL = 0;
    TRISA = 0;
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISB = 0;
    TRISD = 0;
    TRISE = 0;
    PORTD = 0;
    //configuracion interrupciones
    Lcd_Init();
    Lcd_Set_Cursor(1,1);  //linea 1
    Lcd_Write_String("ADC: CONT: TEMP:");
    I2C_Master_Init(100000);        // Inicializar Comuncación I2C
}