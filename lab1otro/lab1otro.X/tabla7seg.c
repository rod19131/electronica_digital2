/*
 * File:   Oscilador.c
 * Author: Alejandro Rodríguez
 *
 * Created on 21 de julio de 2021, 12:16 AM
 */
#include <stdint.h>
#include <xc.h>
#include "tabla7seg.h"

unsigned char tabla7seg(unsigned char numero){
    unsigned char tabla[] = {0b00111111,//0
                         0b00000110,//1
                         0b01011011,//2
                         0b01001111,//3              
                         0b01100110,//4
                         0b01101101,//5
                         0b01111101,//6
                         0b00000111,//7
                         0b01111111,//8
                         0b01101111,//9
                         0b01110111,//A
                         0b01111100,//b
                         0b00111001,//C
                         0b01011110,//d
                         0b01111001,//E
                         0b01110001};//F
    return tabla[numero];
}

