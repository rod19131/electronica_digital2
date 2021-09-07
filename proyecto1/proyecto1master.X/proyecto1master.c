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
//#include <math.h>
#include "I2C.h" 
#include "LCD8bits.h"
#include "usart.h"
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
unsigned char s1, s2, L, R, M, xls, yls, z1 = 0;  //valores esclavos
unsigned char mapping = 0;
int x, y, xms, yms = 0;
unsigned int arct;
int Heading = 0;
char volt[16];        //cadena LCD valores
//float mapear(int adresval){
//    return (adresval-0)*(128-0)/(1200-0)+0;}
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
        switch(s2) {
            //0
            case 0:
                L = 45;
                R = 45;
                break;
            //1
            case 0b00101101:
                L = 94;
                R = 94;
                break;
            //2
            case 0b00000101:
                L = 60;
                R = 60;
                break;
            //3
            case 0b00101000:
                L = 62;
                R = 62;
                break;   
    }   
    if ((PORTBbits.RB0 == 1) && (s1 >= 4)){
            M = 0;
            }    
        
    else if ((PORTBbits.RB0 == 0) || (s1 < 4)){
            M = 1;} 
//        I2C_Master_Start();
//        I2C_Master_Write(0x50);
//        I2C_Master_Write(M);
//        I2C_Master_Stop();
//        __delay_ms(10);
        
        //recepcion adc slave0
        I2C_Master_Start();
        I2C_Master_Write(0x51);
        s1 = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(10);
        
        I2C_Master_Start();
        I2C_Master_Write(0x60);
        I2C_Master_Write(M);
        I2C_Master_Stop();
        __delay_ms(10);
        //recepcion cont slave1
        I2C_Master_Start();
        I2C_Master_Write(0x61);
        s2 = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(10);
        //        //recepcion sensor i2c temp
        I2C_Master_Start();
        I2C_Master_Write(0x1A);
        I2C_Master_Write(0x00);
        I2C_Master_Stop();
        __delay_ms(10);
        
        I2C_Master_Start();
        I2C_Master_Write(0x1B);
        xls = I2C_Master_Read(1);
        xms = I2C_Master_Read(1)<<8;  
        yls = I2C_Master_Read(1);  
        yms = I2C_Master_Read(1)<<8;  
        z1 = I2C_Master_Read(1);  
        z1 = I2C_Master_Read(1); 
        z1 = I2C_Master_Read(0);
        I2C_Master_Stop();
        __delay_ms(50);
        x = xms | xls;
        y = yms | yls;
        if (y <= 900 && x > 0){
            mapping = 192;
        }
        if ((y > 900 && x > 0) || (y > 900 && x < 0)){
            mapping = 128;}
        if (y <= 900 && x < 0){
            mapping = 64;
        }
        if (y < 0 && x < 0){
            mapping = 0;
        }
        if (y < 0 && x > 0){
            mapping = 255;
        }
        //escribir valores en LCD
        sprintf(volt, "%d %c%c %d %d %d\n", s1, L, R, M, y, mapping); //valores para pantalla 2 linea
        enviocadena(volt);                           //envio a pc
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
    TRISB = 1;
    PORTB = 0;
    TRISD = 0;
    TRISE = 0;
    PORTD = 0;
    
    OPTION_REGbits.nRBPU = 0; //pullups habilitados
    WPUBbits.WPUB0 = 1;       //pullups RB0 y RB1 
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
    Lcd_Init();
    Lcd_Set_Cursor(1,1);  //linea 1
    Lcd_Write_String("I_LR_M_MagnetY");
    I2C_Master_Init(100000);        // Inicializar Comuncación I2C
    
    I2C_Master_Start();
    I2C_Master_Write(0x1A);
    I2C_Master_Write(0x0B);
    I2C_Master_Write(0x01);
    I2C_Master_Stop();
    __delay_ms(10);
    
    I2C_Master_Start();
    I2C_Master_Write(0x1A);
    I2C_Master_Write(0x09);
    I2C_Master_Write(0x1D);
    I2C_Master_Stop();
    __delay_ms(10);
    
    I2C_Master_Start();
    I2C_Master_Write(0x1A);
    I2C_Master_Write(0x0A);
    I2C_Master_Write(0x00);
    I2C_Master_Stop();
    __delay_ms(10);
}
