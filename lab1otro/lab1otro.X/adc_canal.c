/*
 * File:   Oscilador.c
 * Author: Alejandro Rodríguez
 *
 * Created on 21 de julio de 2021, 12:16 AM
 */
#include <stdint.h>
#include <xc.h>
#include "adc_canal.h"

unsigned char adc_canal(unsigned char canal) {
    ADCON0bits.CHS = canal;
    return ADRESH;
}


