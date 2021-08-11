/*
 * File:   Oscilador.c
 * Author: Alejandro Rodríguez
 *
 * Created on 21 de julio de 2021, 12:16 AM
 */
#include <stdint.h>
#include <xc.h>
#include "adc_c.h"

void adc_c(void) {
    //configuracion adc
    ADCON0bits.ADCS = 0;//00 se selecciona Fosc/2 para conversion (2us full TAD)
    ADCON0bits.CHS0 = 0;//se selecciona el canal AN0
    ADCON0bits.ADON = 1;//se enciende el adc
    ADCON1bits.VCFG1 = 0;//se ponen los voltajes de referencia internos del PIC
    ADCON1bits.VCFG0 = 0;//0V a 5V
    ADCON1bits.ADFM = 0; //se justifica a la izquierda, vals más significativos
}

