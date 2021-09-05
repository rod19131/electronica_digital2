/* 
 * File:   adc.h
 * Author: Guille Lam
 *
 * Created on 1 de septiembre de 2021, 04:28 PM
 */

#ifndef ADC_H
#define	ADC_H

#pragma config FOSC = INTRC_NOCLKOUT
#include <xc.h> // include processor files - each processor file is guarded.  

void ADC_config(char frec); 

#endif	/* ADC_H */

