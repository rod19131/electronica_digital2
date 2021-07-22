/*
 * Archivo:     lab0.c
 * Dispositivo: PIC16F887
 * Autor:       Jose Alejandro Rodriguez Porras
 * Compilador:  XC8 MPLABX V5.40
 * Programa:    Lab0 juego carrera
 * Hardware:    LEDs en puertos A, B y C, PB en puerto B y 7seg disp en puerto D
 * Creado:      14 de julio de 2021, 05:09 PM
 * Ultima modificacion: 17 de julio de 2021
 */

#include <xc.h>
//CONFIGURATION WORD 1
#pragma config FOSC=INTRC_NOCLKOUT //Oscilador interno sin salida
#pragma config WDTE=OFF           //Reinicio repetitivo del pic
#pragma config PWRTE=OFF           //no espera de 72 ms al iniciar el pic
#pragma config MCLRE=OFF          //El pin MCLR se utiliza como entrada/salida
#pragma config CP=OFF             //Sin protección de código
#pragma config CPD=OFF            //Sin protección de datos
    
#pragma config BOREN=OFF //Sin reinicio cuando el input voltage es inferior a 4V
#pragma config IESO=OFF  //Reinicio sin cambio de reloj de interno a externo
#pragma config FCMEN=OFF //Cambio de reloj externo a interno en caso de fallas
#pragma config LVP=OFF    //Programación en low voltage apagada
    
//CONFIGURATION WORD 2
#pragma config WRT=OFF //Proteccion de autoescritura por el programa desactivada
#pragma config BOR4V=BOR40V //Reinicio abajo de 4V 

//variables
unsigned char inicio = 0;    //inicio
unsigned char a = 0;         //contador de botones jugador 1
unsigned char c = 0;         //contador de botones jugador 2
unsigned char cuentareg = 3; //cuenta regresiva
unsigned char carrera = 0;   //estado carrera
unsigned char j1 = 0;        //boton jugador 1
unsigned char j2 = 0;        //boton jugador 2

//tabla 7 seg
unsigned char tabla[] = {0b00111111,//0
                         0b00000110,//1
                         0b01011011,//2
                         0b01001111 //3              
                        };

unsigned char tabladecadas[] = {0b00000000, //0
                                0b10000000, //1
                                0b01000000, //2
                                0b00100000, //3             
                                0b00010000, //4
                                0b00001000, //5
                                0b00000100, //6
                                0b00000010, //7
                                0b00000001};//8

void __interrupt() isr(void){    // only process timer-triggered interrupts
    //interrupcion por cambio (boton)
    if(RBIF == 1)  {
        //inicio de carrera con antirebote
        if (PORTBbits.RB0 == 0 && carrera == 0) {
            inicio = 1;
            a = 0;      //contador de botones jugador 1
            c = 0;      //contador de botones jugador 2
            cuentareg = 3;
            j1 = 0;
            j2 = 0;
            PORTBbits.RB6 = 0;
            PORTBbits.RB7 = 0;
        }
        //se baja la bandera de interrupcion por cambio
        INTCONbits.RBIF = 0;
    }
    //tmr0
    if (T0IF == 1) {
        //final conteo regresivo
        if (cuentareg == 0 && inicio == 1){
            inicio = 0;
            carrera = 1;
        }
        //conteo regresivo en progreso
        else if (inicio == 1){cuentareg--;}
        TMR0 = 134;
        INTCONbits.T0IF = 0;
    }
}
    
void main(void) {
    //configuracion
    //reloj
    OSCCONbits.IRCF = 1; //125KHz
    OSCCONbits.SCS = 1; //reloj interno
    //configuracion in out
    ANSELH = 0; //Pines digitales
    ANSEL  = 0;
    TRISA  = 0;
    TRISB  = 7;//input RB0, RB1, RB2 y todos los demas pines como out
    TRISC  = 0;
    TRISD  = 0;
    OPTION_REGbits.nRBPU = 0; //pullups habilitados
    WPUBbits.WPUB0 = 1;       //pullups RB0, RB1 y RB2 
    WPUBbits.WPUB1 = 1;
    WPUBbits.WPUB2 = 1;
    PORTA  = 0;               //se limpian los puertos
    PORTB  = 0;
    PORTC  = 0;
    PORTD  = 0;
    //interrupt on change b
    IOCBbits.IOCB0 = 1; //interrupcion por cambio habilitada en RB0, RB1, RB2
    IOCBbits.IOCB1 = 1;
    IOCBbits.IOCB2 = 1;
    //tmr0
    OPTION_REGbits.T0CS = 0; //reloj interno (low to high)
    OPTION_REGbits.PSA  = 0; //prescaler 
    OPTION_REGbits.PS = 7; //111, 1:256
    //reset tmr0
    TMR0 = 134; //para que el tmr0 se reinicie cada 1s
    INTCONbits.T0IF = 0; //baja la bandera de interrupcion del tmr0
    //configuracion interrupciones
    INTCONbits.GIE  = 1; //se habilitan las interrupciones globales
    INTCONbits.RBIE = 1; //interrupcion on change habilitada
    INTCONbits.T0IE = 1; //interrupcion overflow tmr0 habilitada
    while (1){PORTA = tabladecadas[a];//actualiza el cont de decadas del jug 1
              PORTC = tabladecadas[c];//actualiza el cont de decadas del jug 2}
              if (PORTBbits.RB1 == 1) {j1 = 1;} //boton j1 presionado
              if (PORTBbits.RB2 == 1) {j2 = 1;} //boton j2 presionado
              if (PORTBbits.RB1 == 0 && j1 == 1 && carrera == 1){
                  j1 = 0;//antirebote
                  a++; //incremento j1
              }
              if (PORTBbits.RB2 == 0 && j2 == 1 && carrera == 1){
                  j2 = 0;//antirebote
                  c++;   //incremento j2
              }
              if (a != 8 && c != 8){PORTD = tabla[cuentareg];}//disp cuenta reg
              //victoria j1
              if (a == 8){
                  carrera = 0;      //carrera finalizada
                  PORTD = tabla[1]; //disp 1
                  PORTBbits.RB6 = 1;//led victoria j1
              }
              //victoria j2
              if (c == 8){
                  carrera = 0;      //carrera finalizada
                  PORTD = tabla[2]; //disp 2
                  PORTBbits.RB7 = 1;//led victoria j2
              }
              //semaforo
              switch(cuentareg) {
                case 0: 
                    PORTBbits.RB3 = 1;
                    PORTBbits.RB4 = 0;
                    PORTBbits.RB5 = 0; 
                    break;
                case 1:
                    PORTBbits.RB3 = 0;
                    PORTBbits.RB4 = 1;
                    PORTBbits.RB5 = 0;     
                    break;
                case 2:
                    PORTBbits.RB3 = 0;
                    PORTBbits.RB4 = 0;
                    PORTBbits.RB5 = 1; 
                    break;
                case 3:
                    PORTBbits.RB3 = 0;
                    PORTBbits.RB4 = 0;
                    PORTBbits.RB5 = 1;
                    break;}
             }
     }
