/* 
 * File: usart.h  
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef usart_H
#define	usart_H

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