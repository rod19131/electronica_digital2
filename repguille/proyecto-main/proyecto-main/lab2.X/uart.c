/*
 * File:   uart.c
 * Author: Guille Lam
 *
 * Created on 28 de julio de 2021, 02:30 PM
 */


#include <xc.h>
#include "uart.h"

void baud(void){
    SPBRG = 25; 
}

void config_tx(void){
    TXSTAbits.CSRC = 0;
    TXSTAbits.TX9 = 0;
    TXSTAbits.TXEN = 1;
    TXSTAbits.SYNC = 0;
    TXSTAbits.BRGH = 1;
    TXSTAbits.TRMT = 0;
    TXSTAbits.TX9D = 0;
    BAUDCTLbits.BRG16 = 0; //167
    TXREG=0;
}

void config_rx(void){
    RCSTAbits.SPEN = 1;
    RCSTAbits.RX9 = 0;
    RCSTAbits.SREN = 0;
    RCSTAbits.CREN = 1;
    RCREG = 0;
}
void ESC(uint8_t a){
    while(!TXIF);
    TXREG=a;
}
void ESCString(char *a){
    uint8_t i;
    for(i=0;a[i]!='\0';i++){
        ESC(a[i]);
    }
}
uint8_t LEER(){
  while(!RCIF);
  return RCREG;
}
