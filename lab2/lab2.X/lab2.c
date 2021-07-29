/*
 * File:   lab2.c
 * Author: Alejandro Rodríguez
 *
 * Created on 28 de julio de 2021, 01:12 AM
 */
#include <xc.h>
#include <stdint.h>
#include "LCD8bits.h"
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
#define RS RC0
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


void main(void) {
  unsigned int a;
  TRISD = 0x00;
  TRISC = 0x00;
  Lcd_Init();
  while(1)
  {
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("LCD Library for");
    Lcd_Set_Cursor(2,1);
    Lcd_Write_String("MPLAB XC8");
    __delay_ms(500);
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Developed By");
    Lcd_Set_Cursor(2,1);
    Lcd_Write_String("electroSome");
    __delay_ms(500);
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("www.electroSome.com");
    __delay_ms(500);
    Lcd_Clear();
    Lcd_Set_Cursor(2,1);
    Lcd_Write_Char('H');
    Lcd_Write_Char('o');
    Lcd_Write_Char('l');
    Lcd_Write_Char('a');
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("Hola Mundo");
    __delay_ms(1000);
  }
    return;
}

