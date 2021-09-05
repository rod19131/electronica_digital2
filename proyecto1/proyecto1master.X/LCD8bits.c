/* 
 * File: LCD.c  
 * Se utilizó y se adaptaron las librerías de Ligo George 
 * de la página www.electrosome.com
 * Enlace: https://electrosome.com/lcd-pic-mplab-xc8/
 * Revision history: 
 */

//LCD Functions Developed by electroSome

#include "LCD8bits.h"

void Lcd_Port(unsigned char a) {
    PORTD = a;
}

void Lcd_Cmd(unsigned char a) {//
    RS = 0; // => RS = 0 // Dato en el puerto lo va interpretar como comando
    Lcd_Port(a);
    EN = 1; // => E = 1
    __delay_ms(4);
    EN = 0; // => E = 0
}

void Lcd_Clear(void) {
    Lcd_Cmd(0);
    Lcd_Cmd(1);
}

void Lcd_Set_Cursor(unsigned char a,unsigned char b) {//
    unsigned char temp;
    if (a == 1) {
        temp = 0x80 + b - 1;
        Lcd_Cmd(temp);
        
    } else if (a == 2) {
        temp = 0xC0 + b - 1;
        Lcd_Cmd(temp);
    }
}

void Lcd_Init(void) {
    Lcd_Port(0);
    __delay_ms(20);
    Lcd_Cmd(48);
    __delay_ms(5);
    Lcd_Cmd(48);
    __delay_ms(11);
    Lcd_Cmd(48);
    /////////////////////////////////////////////////////
    Lcd_Cmd(56);
    Lcd_Cmd(8);
    Lcd_Cmd(1);
    Lcd_Cmd(6);
    Lcd_Cmd(12);
}

void Lcd_Write_Char(char a) {
    RS = 1; // => RS = 1
    Lcd_Port(a); //Data transfer
    EN = 1;
    __delay_us(40);
    EN = 0;
}

void Lcd_Write_String(char *a) {
    int i;
    for (i = 0; a[i] != '\0'; i++)
        Lcd_Write_Char(a[i]);
}

void Lcd_Shift_Right(void) {
    Lcd_Cmd(28);
}

void Lcd_Shift_Left(void) {
    Lcd_Cmd(24);
}

