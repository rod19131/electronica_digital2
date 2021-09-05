/* 
 * File:   adc.h
 * Author: Guille Lam
 *
 * Created on 2 de septiembre de 2021, 12:26 AM
 */

#ifndef ADC_H
#define	ADC_H

#pragma config FOSC = INTRC_NOCLKOUT
#include <xc.h>

void ADC_config(char frec);

#endif	/* ADC_H */
