/* 
 * File:   uart.h
 * Author: Guille Lam
 *
 * Created on 28 de julio de 2021, 02:17 PM
 */

#ifndef UART_H
#define	UART_H

#include <xc.h> 
#include <stdint.h>

void baud(void);
void config_tx(void);
void config_rx(void);
void ESC(uint8_t a);
void ESCString(char *a);
uint8_t LEER(void);

#endif	/* UART_H */

