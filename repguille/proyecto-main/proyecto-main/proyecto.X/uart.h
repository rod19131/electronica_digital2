/* 
 * File:   uart.h
 * Author: Guille Lam
 *
 * Created on 1 de septiembre de 2021, 05:32 PM
 */

#ifndef uart_H
#define	uart_H

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
//usart
void write(unsigned char data, unsigned char address);
unsigned char read(unsigned char address);
void enviocaracter(char a);
void enviocadena(char* cadena);

#endif	/* LCD_H */
