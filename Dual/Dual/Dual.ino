//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * Con ayuda de: José Guerra
 * Modificaciones y adaptación: Diego Morales
 * IE3027: Electrónica Digital 2 - 2021
 */
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include <SPI.h>
#include <SD.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7};  

//***************************************************************************************************************************************
// VARIABLES
//***************************************************************************************************************************************

//SD  ***********************************************************************************************************************************
File myFile;
/*int x;
int x_seg_ciclo;
int posicion_y = 0;
unsigned char char1;
unsigned char char2;*/
char z;
//int UltimaPocicion=0;
//int primerCero = 0;
const int chipSelect = PA_3;

//GENERAL *******************************************************************************************************************************
int modo = 0;                         //tipo de modo: 0 = menu, 1 = 1V1, 2 = PVE, 3 = Perdiste, 4 = Ganaste 
int CambiaModo = 0;                   //Tipo de modo: 1 = 1V1, 2 = PVE
int bandera_1_vez = 1;                //Se ejecuta una vez TiempoInicial
int bandera_1_vez_modo3 = 0;          //Se ejecuta una vez la imagen y el texto de perder
int bandera_1_vez_bala1 = 0;          //Se coloca el disparo1 = 1 cuando se suelta la bala cargada 1
int bandera_1_vez_bala12 = 0;         //Se coloca el disparo2 = 1 cuando se suelta la bala cargada 2
int bandera_1_vez_bala123 = 0;        //Se coloca el disparo3 = 1 cuando se suelta la bala cargada 3
int bandera_1_vez_bala1234 = 0;       //Se coloca el disparo4 = 1 cuando se suelta la bala cargada 4
int Ataque1 = 0;                      //Mientras bala cargada 1 este en pantalla Ataque1 = 1
int Ataque2 = 0;                      //Mientras bala cargada 2 este en pantalla Ataque2 = 1
int Ataque3 = 0;                      //Mientras bala cargada 3 este en pantalla Ataque3 = 1
int Ataque4 = 0;                      //Mientras bala cargada 4 este en pantalla Ataque4 = 1
String text1 = "PERDISTE";            //Mensaje si se pierde
String text2 = "GANASTE";             //Mensaje si se gana
String text3 = "Esperando Jugador";   //Mensaje en pantalla de espera
int ImagenRandom = 0;                 //Imagen que se colocará si se pierde, este cambiara luego de cierto tiempo
int vida_enemigo = 8;                 //vida del enemigo
int waitingPlayer = 1;                //Esperando a que el otro jugador este listo para comenzar
unsigned long previousMillis = 0;  
const long interval = 42;
int rect = 0;

//COMUNICACION Serial2
int primerCaracter = 0;               //Revisa si se recibio una v de vida o una b de bala
int caracter = 0;                     //El valor de la vida enemiga
int PosX = 0;                         //Posición en x de la bala enemiga recibida
int PosXreflejada;
int caracterx1 = 0;                   //Partes que conforman PosX
int caracterx2 = 0;
int caracterx3 = 0;
int caracterx4 = 0;                   //Caracter de tipo de bala recibida
int tipoBala = 0;                     //Tipo de bala recibida

//cancion

//PERSONAJE ******************************************************************************************************************************
int Per_EjeX = 142;                   //Posicion X del personaje
int Per_EjeY = 98;                    //Posicion Y del personaje
int Posicion = 3010;                  //Posicion del sprite del personaje: moviendose izquierda = 0, quieto = 3010, moviendose derecha = 6020
int Vida = 8;                         //puntos de vida del jugador, 2 puntos es un corazon completo
int Hit = 0;                          //si hubo hit, cargar el sprite con cambio de color a blanco
int Hit_Contador = 0;                 //ayuda a mantener el sprite con cambio de color durante cierto tiempo
int AvisoX = 0;

//Balas **********************************************************************************************************************************
unsigned long TiempoInicial = 0;      //Cuando se presiona por primera vez el botón se revisa el tiempo
unsigned long TiempoAhora = 0;        //Mientras se mantiene presionado se actualiza el valor
int TipoDisparo = 0;                  //Tipo de disparo según el tiempo que se mantuvo presionado el botón: 0 = bala normal, 1 = bala cargada 1, 2 = bala cargada 2, 3 = bala cargada 3, 4 = bala cargada 4

//Balas Enemigas
struct BalaEnemiga{
  int y = 240;                        //Eje Y bala enemiga
  int x;                              //Eje X bala enemiga
  int desaparecer = 0;                //Conteo para hacer desaparecer la bala enemiga si llega hasta abajo
  int tipo = 0;                       //tipo de bala: igual que tipo de disparo
  int hit = 0;                        //Revisa si la bala le dio al jugador
  int width = 5;
} BalaE1, BalaE2, BalaE3, BalaE4, BalaE5, BalaE6;     //******Esto aplica para todas las balas enemigas********

//Balas Aliadas *************************************************************************************************************************
struct BalaAliada{
  int y = 98-15;                      //Eje Y bala aliada          
  int x = 142;                        //Eje X bala aliada
  int disparo = 0;                    //Estado de la bala: 0 = no ha sido disparada, 1 = esta en pantalla, 2 = esta desapareciendo de la pantalla, 3 = tiempo de recuperación
  int desaparecer = 10;               //Conteo para hacer desaparecer la bala aliada si llega hasta arriba
  int recuperacion = 220;             //Conteo para que luego de cierto tiempo la bala se regenere
} BalaA1, BalaA2, BalaA3, BalaA4;     //******Esto aplica para todas las balas aliadas********

//JOYSTICK  *****************************************************************************************************************************
int EjeX;                             //Entrada analógica del joystick
int EjeY;                             //Entrada analógica del joystick
int pushButton = 0;                   //Pushbutton del joystick
int pushButton_Flag = 1;              //Antirrebote pushbutton


//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
void Cargar_Sprite(int width, int height, int x, int y, int posicion, unsigned char bitmap[]);
void Cargar_Sprite_DifColor(int width, int height, int x, int y, int posicion, unsigned char bitmap[], char color1, char color2);

char ConversionDecimal(char x, char y);
char ConversionASCII(char x);
void CargarFondo(int height, int width, int x1, int y1, int negro);
void Sw_Flag(void);

extern uint8_t fondo[];
extern uint8_t MatrizFondo[];
//***************************************************************************************************************************************
// Initialization
//***************************************************************************************************************************************
void setup() {
  //DECLARACIÓN DE PINES JOYSTICK
  pinMode(PE_2, INPUT);
  pinMode(PE_3, INPUT);
  pinMode(PA_7, INPUT_PULLUP);
  pinMode(PC_4, OUTPUT);
  pinMode(PC_5, OUTPUT);

  //Inicialización Pantalla
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  //Inicialización de comunicación serial
  Serial2.begin(9600);
 // Serial4.begin(9600);
  Serial.begin(9600);
  Serial.println("Start");
  LCD_Init();
  LCD_Clear(0x00);

  //Inicialización SD
  SPI.setModule(0);
  Serial.print("Initializing SD card...");
  pinMode(PA_3, OUTPUT);
  
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
    //Cargada de menú
    digitalWrite(PC_4, HIGH);
   myFile = SD.open("Menu.txt", FILE_READ);
      CargarFondo(240, 320, 0, 0, 1);
    // close the file:
    myFile.close();

}
//***************************************************************************************************************************************
// Loop
//***************************************************************************************************************************************
void loop() {
  unsigned long currentMillis = millis();
   
  //Antirrebote
  Sw_Flag();
  if (modo == 0){       //Inicia con Menu
    //Serial4.print("1");
    if (bandera_1_vez_modo3 == 0){
      EjeY = map(analogRead(PE_2),0,4068,500,0);      
      if (EjeY > 400){              //Se revisa si selecciona el recuardo de arriba o abajo
        CambiaModo = 1;
        for (rect = 0; rect<6; rect++){
        Rect(61 + rect, 128 + rect, 198 - 2*rect, 54 - 2*rect, 0x0000);       //Se obscurece el recuadro de abajo
         }
        for (rect = 0; rect<6; rect++){
        Rect(61 + rect, 40 + rect, 197 - 2*rect, 54 - 2*rect, 0xD09D);        //Se aclara el recuadro seleccionado
        }
      }else if (EjeY < 150){
        CambiaModo = 2;
         for (rect = 0; rect<6; rect++){
        Rect(61 + rect, 40 + rect, 197 - 2*rect, 54 - 2*rect, 0x0000); 
         }
         for (rect = 0; rect<6; rect++){
        Rect(61 + rect, 128 + rect, 198 - 2*rect, 54 - 2*rect, 0xD09D); 
         }
      }
    }
    if (pushButton_Flag == 0 &&  CambiaModo == 1 && digitalRead(PA_7) == HIGH){       //Se revisa si se apacha el botón en el modo 1V1
      LCD_Clear(0x00);
      pushButton_Flag = 1;
      bandera_1_vez_modo3 = 1;
      LCD_Print(text3, 15, 100, 2, 0xffff, 0x0000);
      
      
    }else if (pushButton_Flag == 0 &&  CambiaModo == 2 && digitalRead(PA_7) == HIGH){ //Se revisa si se apacha el botón en el modo PVE
      modo = 2;
      pushButton_Flag = 1;
      LCD_Clear(0x00);
      
      }
      
      if (waitingPlayer == 1 && bandera_1_vez_modo3 == 1){          //Si se sigue esperando al jugador
        if (bandera_1_vez_bala1 < 200){
          bandera_1_vez_bala1++;
          FillRect(290, 111, 5, 5, 0xffff);                         //Se crea el primer puntito
        }else if (bandera_1_vez_bala1 < 500){ 
          bandera_1_vez_bala1++;
          FillRect(300, 111, 5, 5, 0xffff);                         //Se crea el segundo puntito
        }else if (bandera_1_vez_bala1 < 800){
          bandera_1_vez_bala1++;
          FillRect(310, 111, 5, 5, 0xffff);                         //Se crea el tercer puntito
        }else {
          bandera_1_vez_bala1 = 0;
          FillRect(300, 111, 5, 5, 0x0000);                         //Se borran los puntitos creados
          FillRect(310, 111, 5, 5, 0x0000);
        }
        Serial2.println("R");                                       //Se envía a la otra tiva que esta listo para jugar
        if (Serial2.available()){   
          primerCaracter = Serial2.read();                          //Se espera el caracter R para saber que el otro jugador esta listo para jugar
          delay(5);
          if (primerCaracter == 'R'){
            waitingPlayer = 0;
            primerCaracter = Serial2.read();
            primerCaracter = Serial2.read();
            while(Serial2.available()){
              primerCaracter = Serial2.read();
            }
            Serial2.flush();
          }
        }
      }else if (waitingPlayer == 0 && bandera_1_vez_modo3 == 1){    //Si el otro jugador esta listo para jugar se inicia el juego
        //Reseteo de las variables utilizadas en el menú
        modo = 1;
        bandera_1_vez_modo3 = 0;
        waitingPlayer = 1;
        bandera_1_vez_bala1 = 0;
        LCD_Clear(0x00);
        
        //Reseteo de variables
        Per_EjeX = 142;
        Per_EjeY = 98;
        Posicion = 3010;
        Vida = 8;
        vida_enemigo = 8;
        Hit = 0;
        Hit_Contador = 0;
        TipoDisparo = 0;
        bandera_1_vez_bala1 = 0;
        bandera_1_vez_bala12 = 0;
        bandera_1_vez_bala123 = 0;
        bandera_1_vez_bala1234 = 0;
        BalaA1.disparo = 0;
        BalaA2.disparo = 0;
        BalaA3.disparo = 0;
        BalaA4.disparo = 0;
        Ataque1 = 0;
        Ataque2 = 0;
        Ataque3 = 0;
        Ataque4 = 0;
        BalaA1.recuperacion = 220;
        BalaA2.recuperacion = 220;
        BalaA3.recuperacion = 220;
        BalaA4.recuperacion = 220;
        BalaE1.y = 240;                                         
        BalaE1.desaparecer = 240-20; 
        BalaE2.y = 240;                           
        BalaE2.desaparecer = 240-20; 
        BalaE3.y = 240;                           
        BalaE3.desaparecer = 240-20; 
        BalaE4.y = 240;                          
        BalaE4.desaparecer = 240-20; 
        BalaE5.y = 240;                          
        BalaE5.desaparecer = 240-20; 
        BalaE6.y = 240;                          
        BalaE6.desaparecer = 240-20; 
        bandera_1_vez = 0;
        
        //Carga de objetos iniciales
        Cargar_Sprite(35, 43, Per_EjeX, Per_EjeY, 3010, NaveEspacial);          //Se carga la nave
        LCD_Bitmap(0, 221, 9, 19, Bala);                                        //Se cargan las 4 balas
        LCD_Bitmap(13, 221, 9, 19, Bala);
        LCD_Bitmap(26, 221, 9, 19, Bala);
        LCD_Bitmap(39, 221, 9, 19, Bala);
        
        //Vida aliada
        Cargar_Sprite(18, 18, 228, 222, 0, Corazones);
        Cargar_Sprite(18, 18, 251, 222, 0, Corazones);
        Cargar_Sprite(18, 18, 274, 222, 0, Corazones);
        Cargar_Sprite(18, 18, 297, 222, 0, Corazones);
  
        //Corazones del enemigo
        Cargar_Sprite_DifColor(18, 18, 180, 222, 0, Corazones, 0x4B, 0x9B);
        Cargar_Sprite_DifColor(18, 18, 157, 222, 0, Corazones, 0x4B, 0x9B);
        Cargar_Sprite_DifColor(18, 18, 134, 222, 0, Corazones, 0x4B, 0x9B);
        Cargar_Sprite_DifColor(18, 18, 111, 222, 0, Corazones, 0x4B, 0x9B);
      }
    }//Finaliza el Menu
    
  else if (modo == 1){          //Modo 1V1
//***************************************************************************************************************************************
// COMUNIACION SERIAL ENTRE TIVAS
//***************************************************************************************************************************************
    //Serial4.print('0');
    digitalWrite(PC_4, LOW);
    if (Serial2.available()){
      primerCaracter = Serial2.read();              //detectar si es v o b para saber con que se esta trabajando
      delay(5);
      if (primerCaracter == 'v'){
        caracter = Serial2.read();
        delay(5);
        vida_enemigo = ConversionASCII(caracter);
        caracter = Serial2.read();
        Serial2.flush();

        //Actualización de la vida enemiga
      if (vida_enemigo == 7){
          Cargar_Sprite_DifColor(18, 18, 111, 222, 648, Corazones, 0x4B, 0x9B);
        }else if (vida_enemigo == 6){
          Cargar_Sprite_DifColor(18, 18, 111, 222, 1296, Corazones, 0x4B, 0x9B);
        }else if (vida_enemigo == 5){
          Cargar_Sprite_DifColor(18, 18, 111, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 134, 222, 648, Corazones, 0x4B, 0x9B);
        }else if (vida_enemigo == 4){
          Cargar_Sprite_DifColor(18, 18, 111, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 134, 222, 1296, Corazones, 0x4B, 0x9B);
        }else if (vida_enemigo == 3){
          Cargar_Sprite_DifColor(18, 18, 111, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 134, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 157, 222, 648, Corazones, 0x4B, 0x9B);
        }else if (vida_enemigo == 2){
          Cargar_Sprite_DifColor(18, 18, 111, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 134, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 157, 222, 1296, Corazones, 0x4B, 0x9B);
        }else if (vida_enemigo == 1){
          Cargar_Sprite_DifColor(18, 18, 111, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 134, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 157, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 180, 222, 648, Corazones, 0x4B, 0x9B);
        }else if (vida_enemigo == 0){
          Cargar_Sprite_DifColor(18, 18, 111, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 134, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 157, 222, 1296, Corazones, 0x4B, 0x9B);
          Cargar_Sprite_DifColor(18, 18, 180, 222, 1296, Corazones, 0x4B, 0x9B);
          modo = 4;
        }
      }else if (primerCaracter == 'b'){
        caracterx1 = Serial2.read();                //Detectar cuantos caracteres tiene la posicion en x recibida
        delay(5);
        caracterx2 = Serial2.read();
        delay(5);
        if (caracterx2 == ','){
          PosXreflejada = ConversionASCII(caracterx1);
          PosX = 320 - PosXreflejada;
        }else {
          caracterx3 = Serial2.read();
          delay(5);
          if (caracterx3 == ','){
            PosXreflejada = ConversionASCII(caracterx1)*10 + ConversionASCII(caracterx2);
            PosX = 320 - PosXreflejada;
          }else{
            PosXreflejada = ConversionASCII(caracterx1)*100 + ConversionASCII(caracterx2)*10 + ConversionASCII(caracterx3);
            PosX = 320 - PosXreflejada;
            delay(5);
            caracterx4 = Serial2.read();
          }
        }
        caracterx4 = Serial2.read();
        delay(5);
        tipoBala = ConversionASCII(caracterx4);
        caracterx4 = Serial2.read();
        delay(5);
        caracterx4 = Serial2.read();
        Serial2.flush();
        if (tipoBala == 1){
          PosX = PosX - 11;
        }else if (tipoBala == 2){
          PosX = PosX - 17;
        }else if (tipoBala == 3){
          PosX = PosX - 23;
        }else if (tipoBala == 4){
          PosX = PosX - 29;
        }
        //Colocación del valores leidos en la info de balas correspondientes y la vida enemiga
        if (BalaE1.desaparecer == 240-20 && BalaE1.y == 240){
          BalaE1.y = 0;
          BalaE1.x = PosX;
          BalaE1.tipo = tipoBala;
          Serial.println("Bala 1");
        }else if (BalaE2.desaparecer == 240-20 && BalaE2.y == 240){
          BalaE2.y = 0;
          BalaE2.x = PosX;
          BalaE2.tipo = tipoBala;
          Serial.println("Bala 2");
        }else if (BalaE3.desaparecer == 240-20 && BalaE3.y == 240){
          BalaE3.y = 0;
          BalaE3.x = PosX;
          BalaE3.tipo = tipoBala;
          Serial.println("Bala 3");
        }else if (BalaE4.desaparecer == 240-20 && BalaE4.y == 240){
          BalaE4.y = 0;
          BalaE4.x = PosX;
          BalaE4.tipo = tipoBala;
          Serial.println("Bala 4");
        }else if (BalaE5.desaparecer == 240-20 && BalaE5.y == 240){
          BalaE5.y = 0;
          BalaE5.x = PosX;
          BalaE5.tipo = tipoBala;
          Serial.println("Bala 5");
        }else if (BalaE6.desaparecer == 240-20 && BalaE6.y == 240){
          BalaE6.y = 0;
          BalaE6.x = PosX;
          BalaE6.tipo = tipoBala;
          Serial.println("Bala 6");
        }
         //definición del grosor de cada bala según su tipo
        if (BalaE1.tipo == 0){
          BalaE1.width = 5;
        }else if (BalaE1.tipo == 1){
          BalaE1.width = 11;
        }else if (BalaE1.tipo == 2){
          BalaE1.width = 17;
        }else if (BalaE1.tipo == 3){
          BalaE1.width = 23;
        }else if (BalaE1.tipo == 4){
          BalaE1.width = 29;
        }
        
        if (BalaE2.tipo == 0){
          BalaE2.width = 5;
        }else if (BalaE2.tipo == 1){
          BalaE2.width = 11;
        }else if (BalaE2.tipo == 2){
          BalaE2.width = 17;
        }else if (BalaE2.tipo == 3){
          BalaE2.width = 23;
        }else if (BalaE2.tipo == 4){
          BalaE2.width = 29;
        }
        
        if (BalaE3.tipo == 0){
          BalaE3.width = 5;
        }else if (BalaE3.tipo == 1){
          BalaE3.width = 11;
        }else if (BalaE3.tipo == 2){
          BalaE3.width = 17;
        }else if (BalaE3.tipo == 3){
          BalaE3.width = 23;
        }else if (BalaE3.tipo == 4){
          BalaE3.width = 29;
        }
        
        if (BalaE4.tipo == 0){
          BalaE4.width = 5;
        }else if (BalaE4.tipo == 1){
          BalaE4.width = 11;
        }else if (BalaE4.tipo == 2){
          BalaE4.width = 17;
        }else if (BalaE4.tipo == 3){
          BalaE4.width = 23;
        }else if (BalaE4.tipo == 4){
          BalaE4.width = 29;
        }
        
        if (BalaE5.tipo == 0){
          BalaE5.width = 5;
        }else if (BalaE5.tipo == 1){
          BalaE5.width = 11;
        }else if (BalaE5.tipo == 2){
          BalaE5.width = 17;
        }else if (BalaE5.tipo == 3){
          BalaE5.width = 23;
        }else if (BalaE5.tipo == 4){
          BalaE5.width = 29;
        }
        
        if (BalaE6.tipo == 0){
          BalaE6.width = 5;
        }else if (BalaE6.tipo == 1){
          BalaE6.width = 11;
        }else if (BalaE6.tipo == 2){
          BalaE6.width = 17;
        }else if (BalaE6.tipo == 3){
          BalaE6.width = 23;
        }else if (BalaE6.tipo == 4){
          BalaE6.width = 29;
        }
      }
    }

//***************************************************************************************************************************************
// HIT AL PERSONAJE
//***************************************************************************************************************************************
    //Colision de las balas con el personaje
    if ((Per_EjeX < BalaE1.x + BalaE1.width) && (Per_EjeX + 35 > BalaE1.x) && (Per_EjeY < BalaE1.y + 6) && (Per_EjeY + 43 > BalaE1.y)){  
      BalaE1.hit = 1;
    }
    
    if ((Per_EjeX < BalaE2.x + BalaE2.width) && (Per_EjeX + 35 > BalaE2.x) && (Per_EjeY < BalaE2.y + 6) && (Per_EjeY + 43 > BalaE2.y)){      
      BalaE2.hit = 1;
    }
    if ((Per_EjeX < BalaE3.x + BalaE3.width) && (Per_EjeX + 35 > BalaE3.x) && (Per_EjeY < BalaE3.y + 6) && (Per_EjeY + 43 > BalaE3.y)){      
      BalaE3.hit = 1;
    }
    if ((Per_EjeX < BalaE4.x + BalaE4.width) && (Per_EjeX + 35 > BalaE4.x) && (Per_EjeY < BalaE4.y + 6) && (Per_EjeY + 43 > BalaE4.y)){      
      BalaE4.hit = 1;
    }
    if ((Per_EjeX < BalaE5.x + BalaE5.width) && (Per_EjeX + 35 > BalaE5.x) && (Per_EjeY < BalaE5.y + 6) && (Per_EjeY + 43 > BalaE5.y)){      
      BalaE5.hit = 1;
    }
    if ((Per_EjeX < BalaE6.x + BalaE6.width) && (Per_EjeX + 35 > BalaE6.x) && (Per_EjeY < BalaE6.y + 6) && (Per_EjeY + 43 > BalaE6.y)){      
      BalaE6.hit = 1;
    }

    if (BalaE1.hit == 1 || BalaE2.hit == 1 || BalaE3.hit == 1 || BalaE4.hit == 1 || BalaE5.hit == 1 || BalaE6.hit == 1){        //Si alguna bala le dio al jugador
      //Revisar que bala le dio
      if (BalaE1.hit == 1){
        FillRect(BalaE1.x, BalaE1.y - 6, BalaE1.width, 11, 0x000);
          BalaE1.y = 240;
          BalaE1.desaparecer = 240-20;
          BalaE1.hit = 0;
          if (BalaE1.tipo == 0){         
            Vida--;
            }else{
              Vida = Vida - BalaE1.tipo * 2;
            }
        }else if (BalaE2.hit == 1){
          FillRect(BalaE2.x, BalaE2.y - 6, BalaE2.width, 11, 0x000);
          BalaE2.y = 240;
          BalaE2.desaparecer = 240-20;
          BalaE2.hit = 0;
          if (BalaE2.tipo == 0){         
            Vida--;
            }else{
              Vida = Vida - BalaE2.tipo * 2;
            }
          }else if (BalaE3.hit == 1){
          FillRect(BalaE3.x, BalaE3.y - 6, BalaE3.width, 11, 0x000);
          BalaE3.y = 240;
          BalaE3.desaparecer = 240-20;
          BalaE3.hit = 0;
          if (BalaE3.tipo == 0){         
            Vida--;
            }else{
              Vida = Vida - BalaE3.tipo * 2;
            }
          }else if (BalaE4.hit == 1){
          FillRect(BalaE4.x, BalaE4.y - 6, BalaE4.width, 11, 0x000);
          BalaE4.y = 240;
          BalaE4.desaparecer = 240-20;
          BalaE4.hit = 0;
          if (BalaE4.tipo == 0){         
            Vida--;
            }else{
              Vida = Vida - BalaE4.tipo * 2;
            }
          }else if (BalaE5.hit == 1){
          FillRect(BalaE5.x, BalaE5.y - 6, BalaE5.width, 11, 0x000);
          BalaE5.y = 240;
          BalaE5.desaparecer = 240-20;
          BalaE5.hit = 0;
          if (BalaE5.tipo == 0){         
            Vida--;
            }else{
              Vida = Vida - BalaE5.tipo * 2;
            }
          }else if (BalaE6.hit == 1){
          FillRect(BalaE6.x, BalaE6.y - 6, BalaE6.width, 11, 0x000);
          BalaE6.y = 240;
          BalaE6.desaparecer = 240-20;
          BalaE6.hit = 0;
          if (BalaE6.tipo == 0){         
            Vida--;
          }else{
              Vida = Vida - BalaE6.tipo * 2;
            }
          }  
     
      if (Vida < 0){
        Vida = 0;
      }

      Serial2.println("v"+String(Vida));            //Envio de vida a la otra tiva
      Hit = 1;
      Hit_Contador = 0;
      
      //Actualización de la vida
      if (Vida == 7){
        Cargar_Sprite(18, 18, 228, 222, 648, Corazones);
      }else if (Vida == 6){
        Cargar_Sprite(18, 18, 228, 222, 1296, Corazones);
      }else if (Vida == 5){
        Cargar_Sprite(18, 18, 228, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 251, 222, 648, Corazones);
      }else if (Vida == 4){
        Cargar_Sprite(18, 18, 228, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 251, 222, 1296, Corazones);
      }else if (Vida == 3){
        Cargar_Sprite(18, 18, 228, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 251, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 274, 222, 648, Corazones);
      }else if (Vida == 2){
        Cargar_Sprite(18, 18, 228, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 251, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 274, 222, 1296, Corazones);
      }else if (Vida == 1){
        Cargar_Sprite(18, 18, 228, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 251, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 274, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 297, 222, 648, Corazones);
      }else if (Vida == 0){
        Cargar_Sprite(18, 18, 228, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 251, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 274, 222, 1296, Corazones);
        Cargar_Sprite(18, 18, 297, 222, 1296, Corazones);
        modo = 3;
      }
    }

//***************************************************************************************************************************************
// COMPORTAMIENTO DE BALAS ENEMIGAS
//**************************************************************************************************************************************
    //Bala enemiga 1
    if (BalaE1.y < 211 ){                               //Mientras no ha llegado hasta abajo de la pantalla
      if (BalaE1.y < 6){                                //Si esta apareciendo la bala enemiga pintar solo líneas
        H_line(BalaE1.x, BalaE1.y, BalaE1.width, 0x0119);
        BalaE1.y++;
        
      }else{                                            //Si ya apareció la bala pintar el rectangulo correspondiente
        if (BalaE1.tipo == 4){
          FillRect(BalaE1.x, BalaE1.y, 29, 6, 0x0119);
          H_line(BalaE1.x, BalaE1.y - 6, 29, 0x0000);
          H_line(BalaE1.x, BalaE1.y - 5, 29, 0x0000);
          H_line(BalaE1.x, BalaE1.y - 4, 29, 0x0000);
        }else{
          FillRect(BalaE1.x, BalaE1.y, BalaE1.width, 6, 0x0119);
          H_line(BalaE1.x, BalaE1.y - 6, BalaE1.width, 0x0000);
          H_line(BalaE1.x, BalaE1.y - 5, BalaE1.width, 0x0000);
        }
        
        if (BalaE1.tipo == 4){
          BalaE1.y = BalaE1.y + 3;
        }else{
          BalaE1.y = BalaE1.y + 2;
        }
      }
      if (BalaE1.y == 210){                               //Si esta a punto de llegar hasta abajo
        BalaE1.desaparecer = 205;
      }
    }else if (BalaE1.desaparecer != 240-20){              //Si ya llego hasta abajo, pintar línas de negro para simular que desaparece
        BalaE1.y = 240;
        H_line(BalaE1.x, BalaE1.desaparecer, BalaE1.width, 0x0000);
        BalaE1.desaparecer++;
      }

   //Bala enemiga 2
   if (BalaE2.y < 211 ){
    if (BalaE2.y < 6){
      H_line(BalaE2.x, BalaE2.y, BalaE2.width, 0x0119);
      BalaE2.y++;
      }else{
        if (BalaE2.tipo == 4){
          FillRect(BalaE2.x, BalaE2.y, 29, 6, 0x0119);
          H_line(BalaE2.x, BalaE2.y - 6, 29, 0x0000);
          H_line(BalaE2.x, BalaE2.y - 5, 29, 0x0000);
          H_line(BalaE2.x, BalaE2.y - 4, 29, 0x0000);
        }else{
          FillRect(BalaE2.x, BalaE2.y, BalaE2.width, 6, 0x0119);
          H_line(BalaE2.x, BalaE2.y - 6, BalaE2.width, 0x0000);
          H_line(BalaE2.x, BalaE2.y - 5, BalaE2.width, 0x0000);
        }
        
        if (BalaE2.tipo == 4){
          BalaE2.y = BalaE2.y + 3;
        }else{
          BalaE2.y = BalaE2.y + 2;
        }
      }
      if (BalaE2.y == 210){
        BalaE2.desaparecer = 205;
      }
    }else if (BalaE2.desaparecer != 240-20){
        BalaE2.y = 240;
        H_line(BalaE2.x, BalaE2.desaparecer, BalaE2.width, 0x0000);
        BalaE2.desaparecer++;
      }

   //Bala enemiga 3
   if (BalaE3.y < 211 ){
      if (BalaE3.y < 6){
        H_line(BalaE3.x, BalaE3.y, BalaE3.width, 0x0119);
        BalaE3.y++;
      }else{
        if (BalaE3.tipo == 4){
          FillRect(BalaE3.x, BalaE3.y, 29, 6, 0x0119);
          H_line(BalaE3.x, BalaE3.y - 6, 29, 0x0000);
          H_line(BalaE3.x, BalaE3.y - 5, 29, 0x0000);
          H_line(BalaE3.x, BalaE3.y - 4, 29, 0x0000);
        }else{
          FillRect(BalaE3.x, BalaE3.y, BalaE3.width, 6, 0x0119);
          H_line(BalaE3.x, BalaE3.y - 6, BalaE3.width, 0x0000);
          H_line(BalaE3.x, BalaE3.y - 5, BalaE3.width, 0x0000);
        }
        
        if (BalaE3.tipo == 4){
          BalaE3.y = BalaE3.y + 3;
        }else{
          BalaE3.y = BalaE3.y + 2;
        }
      }
      if (BalaE3.y == 210){
        BalaE3.desaparecer = 205;
      }
    }else if (BalaE3.desaparecer != 240-20){
        BalaE3.y = 240;
        H_line(BalaE3.x, BalaE3.desaparecer, BalaE3.width, 0x0000);
        BalaE3.desaparecer++;
      }

   //Bala enemiga 4
   if (BalaE4.y < 211 ){
      if (BalaE4.y < 6){
        H_line(BalaE4.x, BalaE4.y, BalaE4.width, 0x0119);
        BalaE4.y++;
      }else{
       if (BalaE4.tipo == 4){
          FillRect(BalaE4.x, BalaE4.y, 29, 6, 0x0119);
          H_line(BalaE4.x, BalaE4.y - 6, 29, 0x0000);
          H_line(BalaE4.x, BalaE4.y - 5, 29, 0x0000);
          H_line(BalaE4.x, BalaE4.y - 4, 29, 0x0000);
        }else{
          FillRect(BalaE4.x, BalaE4.y, BalaE4.width, 6, 0x0119);
          H_line(BalaE4.x, BalaE4.y - 6, BalaE4.width, 0x0000);
          H_line(BalaE4.x, BalaE4.y - 5, BalaE4.width, 0x0000);
        }
        
        if (BalaE4.tipo == 4){
          BalaE4.y = BalaE4.y + 3;
        }else{
          BalaE4.y = BalaE4.y + 2;
        }
      }
      if (BalaE4.y == 210){
        BalaE4.desaparecer = 205;
      }
    }else if (BalaE4.desaparecer != 240-20){
        BalaE4.y = 240;
        H_line(BalaE4.x, BalaE4.desaparecer, BalaE4.width, 0x0000);
        BalaE4.desaparecer++;
      }

      //Bala enemiga 5
   if (BalaE5.y < 211 ){
      if (BalaE5.y < 6){
        H_line(BalaE5.x, BalaE5.y, BalaE5.width, 0x0119);
        BalaE5.y++;
      }else{
       if (BalaE5.tipo == 4){
          FillRect(BalaE5.x, BalaE5.y, 29, 6, 0x0119);
          H_line(BalaE5.x, BalaE5.y - 6, 29, 0x0000);
          H_line(BalaE5.x, BalaE5.y - 5, 29, 0x0000);
          H_line(BalaE5.x, BalaE5.y - 4, 29, 0x0000);
        }else{
          FillRect(BalaE5.x, BalaE5.y, BalaE5.width, 6, 0x0119);
          H_line(BalaE5.x, BalaE5.y - 6, BalaE5.width, 0x0000);
          H_line(BalaE5.x, BalaE5.y - 5, BalaE5.width, 0x0000);
        }
        
        if (BalaE5.tipo == 4){
          BalaE5.y = BalaE5.y + 3;
        }else{
          BalaE5.y = BalaE5.y + 2;
        }
      }
      if (BalaE5.y == 210){
        BalaE5.desaparecer = 205;
      }
    }else if (BalaE5.desaparecer != 240-20){
        BalaE5.y = 240;
        H_line(BalaE5.x, BalaE5.desaparecer, BalaE5.width, 0x0000);
        BalaE5.desaparecer++;
      }

      //Bala enemiga 6
   if (BalaE6.y < 211 ){
      if (BalaE6.y < 6){
        H_line(BalaE6.x, BalaE6.y, BalaE6.width, 0x0119);
        BalaE6.y++;
      }else{
       if (BalaE6.tipo == 4){
          FillRect(BalaE6.x, BalaE6.y, 29, 6, 0x0119);
          H_line(BalaE6.x, BalaE6.y - 6, 29, 0x0000);
          H_line(BalaE6.x, BalaE6.y - 5, 29, 0x0000);
          H_line(BalaE6.x, BalaE6.y - 4, 29, 0x0000);
        }else{
          FillRect(BalaE6.x, BalaE6.y, BalaE6.width, 6, 0x0119);
          H_line(BalaE6.x, BalaE6.y - 6, BalaE6.width, 0x0000);
          H_line(BalaE6.x, BalaE6.y - 5, BalaE6.width, 0x0000);
        }
        
        if (BalaE6.tipo == 4){
          BalaE6.y = BalaE6.y + 3;
        }else{
          BalaE6.y = BalaE6.y + 2;
        }
      }
      if (BalaE6.y == 210){
        BalaE6.desaparecer = 205;
      }
    }else if (BalaE6.desaparecer != 240-20){
        BalaE6.y = 240;
        H_line(BalaE6.x, BalaE6.desaparecer, BalaE6.width, 0x0000);
        BalaE6.desaparecer++;
      }

//***************************************************************************************************************************************
// COMPORTAMIENTO DE BALAS ALIADAS
//***************************************************************************************************************************************
//Se revisa que tipo de disparo desea el jugador*****************************************************************************************
    if(digitalRead(PA_7) == LOW && pushButton_Flag == 0 &&  Ataque1 == 0 && Ataque2 == 0 && Ataque3 == 0 && Ataque4 == 0 && BalaA1.recuperacion == 220 && BalaA2.recuperacion == 220 && BalaA3.recuperacion == 220 && BalaA4.recuperacion == 220 ){        //Se lee si se deja presionado el botón
      TiempoAhora = millis();
      if (TiempoAhora - TiempoInicial < 500 &&  Ataque1 == 0 && Ataque2 == 0 && Ataque3 == 0 && Ataque4 == 0){           //Si es menor, se mantiene 
        //TipoDisparo = 0;
      }else if (TiempoAhora - TiempoInicial <= 1500 && Ataque1 == 0 && Ataque2 == 0 && Ataque3 == 0 && Ataque4 == 0){
        TipoDisparo = 1;
      }else if (TiempoAhora - TiempoInicial < 2500 && Ataque1 == 0 && Ataque2 == 0 && Ataque3 == 0 && Ataque4 == 0){
        TipoDisparo = 2;
      }else if (TiempoAhora - TiempoInicial < 3500 && Ataque1 == 0 && Ataque2 == 0 && Ataque3 == 0 && Ataque4 == 0){
        TipoDisparo = 3;
      }else if (TiempoAhora - TiempoInicial > 4000 && Ataque1 == 0 && Ataque2 == 0 && Ataque3 == 0 && Ataque4 == 0){
        TipoDisparo = 4;
      }
    }else{
      TiempoAhora = 0;
      TiempoInicial = 0;
      bandera_1_vez = 0;
    }

//Se revisa la posición del jugador*********************************************************************************************
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; 
      EjeY = map(analogRead(PE_2),0,4068,500,0);              //Mapeo de las entradas analógicas del joystick
      EjeX = map(analogRead(PE_3),0,4068,0,500);
      if (EjeX > 260){                                        //Se revisa si va hacia la izquierda
        AvisoX = 1;
        V_line(Per_EjeX+35, Per_EjeY, 43, 0x0000);
        V_line(Per_EjeX+34, Per_EjeY, 43, 0x0000);
        V_line(Per_EjeX+33, Per_EjeY, 43, 0x0000);
        V_line(Per_EjeX+32, Per_EjeY, 43, 0x0000);
        if (Per_EjeX <= 0){
          Per_EjeX = 3;
          AvisoX = 0;
        }
        Per_EjeX = Per_EjeX - 3;
        Posicion = 0;
        
      }else if (EjeX < 200){                                  //Se revisa si va hacia la derecha
        AvisoX = 1;
        V_line(Per_EjeX, Per_EjeY, 43, 0x0000);
        V_line(Per_EjeX+1, Per_EjeY, 43, 0x0000);
        V_line(Per_EjeX+2, Per_EjeY, 43, 0x0000);
        V_line(Per_EjeX+3, Per_EjeY, 43, 0x0000);
        if (Per_EjeX >= 281){
          Per_EjeX = 278;
          AvisoX = 0;
        }
        Per_EjeX = Per_EjeX + 3;
        Posicion = 6020;
        
      }else{
        Posicion = 3010;
      }
      
      if (EjeY > 260){
        if (AvisoX == 1){
          H_line(Per_EjeX-3, Per_EjeY+43, 35, 0x0000);
          H_line(Per_EjeX-3, Per_EjeY+42, 35, 0x0000);
          H_line(Per_EjeX-3, Per_EjeY+41, 35, 0x0000);
          H_line(Per_EjeX-3, Per_EjeY+40, 35, 0x0000);
          AvisoX = 0;
        }else{
          H_line(Per_EjeX, Per_EjeY+43, 35, 0x0000);
          H_line(Per_EjeX, Per_EjeY+42, 35, 0x0000);
          H_line(Per_EjeX, Per_EjeY+41, 35, 0x0000);
          H_line(Per_EjeX, Per_EjeY+40, 35, 0x0000);
        }
        
        if (Per_EjeY <= 21){
          Per_EjeY = 24;
        }
        Per_EjeY = Per_EjeY - 3;
      }else if (EjeY < 200){
        if (AvisoX == 1){
          FillRect(Per_EjeX-3, Per_EjeY-4, 35, 7, 0x000);
          AvisoX = 0;
        }else{
          FillRect(Per_EjeX, Per_EjeY-4, 35, 7, 0x000);
        }
        if (Per_EjeY >= 178){
          Per_EjeY = 175;
        }
        Per_EjeY = Per_EjeY + 3;
      }
      //Rect(Per_EjeX, Per_EjeY,35, 43,0x0000);
      
  //***************************************************************************************************************************************
  // IMPRESIÓN DEL JUGADOR
  //***************************************************************************************************************************************
        
      if (Hit == 0){                     //Si no se recibe ningun golpe se usa el sprite normal
        Cargar_Sprite(35, 43, Per_EjeX, Per_EjeY, Posicion, NaveEspacial);
      }else{                            //Si se recibió un golpe se debe mantener el sprite en blanco durante cierto tiempo
        if (Hit_Contador == 20){
          Hit = 0;
        }
        Cargar_Sprite_DifColor(35, 43, Per_EjeX, Per_EjeY, Posicion, NaveEspacial, 255, 255);
        Hit_Contador++;
      }
    }

//Comportamiento de cada bala según el tipo de disparo********************************************************************************
    if (TipoDisparo == 0){
      //Accion del Pushbutton
      if (pushButton_Flag == 0 && digitalRead(PA_7) == HIGH ){          //Se revisa si el jugador disparo
        pushButton_Flag = 1;
        if (BalaA1.disparo == 0){                       //Se revisa que bala esta disponible para dispararla
          BalaA1.y = Per_EjeY - 20;
          BalaA1.x = Per_EjeX+15;
          BalaA1.disparo = 1;
          BalaA1.recuperacion = 0;
        }else if (BalaA2.disparo == 0){
          BalaA2.y = Per_EjeY - 20;
          BalaA2.x = Per_EjeX+15;
          BalaA2.disparo = 1;
          BalaA2.recuperacion = 0;
        }else if (BalaA3.disparo == 0){
          BalaA3.y = Per_EjeY - 20;
          BalaA3.x = Per_EjeX+15;
          BalaA3.disparo = 1;
          BalaA3.recuperacion = 0;
        }else if (BalaA4.disparo == 0){
          BalaA4.y = Per_EjeY - 20;
          BalaA4.x = Per_EjeX+15;
          BalaA4.disparo = 1;
          BalaA4.recuperacion = 0;
        }
      }

      //Disparo 1
      if (BalaA1.disparo == 1){                                           //Se genera la bala disparada
        digitalWrite(PC_5, HIGH);
        FillRect(BalaA1.x, BalaA1.y, 5, 10, 0x1681);                      //Se coloca un rectangulo de color 
        H_line(BalaA1.x, BalaA1.y + 10, 5, 0x0000);                       //Se coloca una linea negra hasta abajo para simular el movimiento
        H_line(BalaA1.x, BalaA1.y + 9, 5, 0x0000);
        BalaA1.y = BalaA1.y - 2;
        if (BalaA1.y <= 0){                                               //Si llega hasta el tope de la pantalla por la parte de arriba pasa al siguiente estado
          BalaA1.disparo = 2;
          BalaA1.desaparecer = 11;
        digitalWrite(PC_5, LOW);
        }
      }else if (BalaA1.disparo == 2){                                     //Se continua con la idea de la linea negra para dar al sesación que sesaparece
        H_line(BalaA1.x, BalaA1.desaparecer , 5, 0x0000);
        if (BalaA1.desaparecer  == 0){                                     //Cuando se elimina la bala por completo pasa al siguiente estado
          BalaA1.disparo = 3;
          Serial2.println("b" + String(BalaA1.x) + "," + "0");
        }
        BalaA1.desaparecer--;
      }else if(BalaA1.disparo == 3){                                      //Modo recuperación, contador para que luego de un tiempo se regenere la bala
        BalaA1.recuperacion++;
        if (BalaA1.recuperacion == 220){
          BalaA1.disparo = 0;
        }
      }
  
      //Disparo 2
      if (BalaA2.disparo == 1){
        digitalWrite(PC_5, HIGH);
        FillRect(BalaA2.x, BalaA2.y, 5, 10, 0x1681);                      
        H_line(BalaA2.x, BalaA2.y + 10, 5, 0x0000);                       
        H_line(BalaA2.x, BalaA2.y + 9, 5, 0x0000);
        BalaA2.y = BalaA2.y - 2;
        if (BalaA2.y <= 0){                                               
          BalaA2.disparo = 2;
          BalaA2.desaparecer = 11;
          digitalWrite(PC_5, LOW);
        }
      }else if (BalaA2.disparo == 2){
        H_line(BalaA2.x, BalaA2.desaparecer, 5, 0x0000);
        if (BalaA2.desaparecer == 0){
          BalaA2.disparo = 3;
          Serial2.println("b" + String(BalaA2.x) + "," + "0");
        }
        BalaA2.desaparecer--;
      }else if(BalaA2.disparo == 3){
        BalaA2.recuperacion++;
        if (BalaA2.recuperacion == 220){
          BalaA2.disparo = 0;
        }
      }
  
      //Disparo 3
      if (BalaA3.disparo == 1){
        digitalWrite(PC_5, HIGH);
        FillRect(BalaA3.x, BalaA3.y, 5, 10, 0x1681);                      
        H_line(BalaA3.x, BalaA3.y + 10, 5, 0x0000);                       
        H_line(BalaA3.x, BalaA3.y + 9, 5, 0x0000);
        BalaA3.y = BalaA3.y - 2;
        if (BalaA3.y <= 0){                                               
          BalaA3.disparo = 2;
          BalaA3.desaparecer = 11;
          digitalWrite(PC_5, LOW);
        }
      }else if (BalaA3.disparo == 2){
        H_line(BalaA3.x, BalaA3.desaparecer, 5, 0x0000);
        if (BalaA3.desaparecer == 0){
          BalaA3.disparo = 3;
          Serial2.println("b" + String(BalaA3.x) + "," + "0");
        }
        BalaA3.desaparecer--;
      }else if(BalaA3.disparo == 3){
        BalaA3.recuperacion++;
        if (BalaA3.recuperacion == 220){
          BalaA3.disparo = 0;
        }
      }
  
      //Disparo 4
      if (BalaA4.disparo == 1){
        digitalWrite(PC_5, HIGH);
        FillRect(BalaA4.x, BalaA4.y, 5, 10, 0x1681);                      
        H_line(BalaA4.x, BalaA4.y + 10, 5, 0x0000);                       
        H_line(BalaA4.x, BalaA4.y + 9, 5, 0x0000);
        BalaA4.y = BalaA4.y - 2;
        if (BalaA4.y <= 0){                                               
          BalaA4.disparo = 2;
          BalaA4.desaparecer = 11;
          digitalWrite(PC_5, LOW);
        }
      }else if (BalaA4.disparo == 2){
        H_line(BalaA4.x, BalaA4.desaparecer, 5, 0x0000);
        if (BalaA4.desaparecer == 0){
          BalaA4.disparo = 3;
          Serial2.println("b" + String(BalaA4.x) + "," + "0");
        }
        BalaA4.desaparecer--;
      }else if(BalaA4.disparo == 3){
        BalaA4.recuperacion++;
        if (BalaA4.recuperacion == 220){
          BalaA4.disparo = 0;
        }
      }
      //Termina TipoDisparo = 0
      
    }else if (TipoDisparo == 1){
      if (bandera_1_vez_bala1 == 0){
          BalaA1.disparo = 1;
        }
        if (digitalRead(PA_7) == HIGH){
          Ataque1 = 1;
          pushButton_Flag = 1;
        }
      if (Ataque1 == 1){
        if (bandera_1_vez_bala1 == 0){
          bandera_1_vez_bala1 = 1;
          BalaA1.y = Per_EjeY - 20;
          BalaA1.x = Per_EjeX+12;
        }
        if (BalaA1.disparo == 1){
          FillRect(BalaA1.x, BalaA1.y, 11, 10, 0x1681);                      
          H_line(BalaA1.x, BalaA1.y + 10, 11, 0x0000);                       
          H_line(BalaA1.x, BalaA1.y + 9, 11, 0x0000);
          BalaA1.y = BalaA1.y - 2;
          if (BalaA1.y <= 0){                                               
            BalaA1.disparo = 2;
            BalaA1.desaparecer = 11;
          }
        }else if (BalaA1.disparo == 2){
          H_line(BalaA1.x, BalaA1.desaparecer , 11, 0x0000);
          if (BalaA1.desaparecer  == 0){
            BalaA1.disparo = 3;
            BalaA1.recuperacion = 0;
            Serial2.println("b" + String(BalaA1.x) + "," + "1");
          }
          BalaA1.desaparecer--;
        }else if(BalaA1.disparo == 3){
          BalaA1.recuperacion++;
          if (BalaA1.recuperacion == 300){
            BalaA1.recuperacion = 220;
            BalaA1.disparo = 0;
            TipoDisparo = 0;
            bandera_1_vez_bala1 = 0;
            Ataque1 = 0;
           }
         }           
       }
     
    }else if (TipoDisparo == 2){
      if (bandera_1_vez_bala12 == 0){
          BalaA2.disparo = 1;
        }
        if (digitalRead(PA_7) == HIGH){
          Ataque2 = 1;
          pushButton_Flag = 1;
        }
      if (Ataque2 == 1){
        if (bandera_1_vez_bala12 == 0){
          bandera_1_vez_bala12 = 1;
          BalaA2.y = Per_EjeY - 20;
          BalaA2.x = Per_EjeX+9;
        }
        if (BalaA2.disparo == 1){
          FillRect(BalaA2.x, BalaA2.y, 17, 10, 0x1681);                      
          H_line(BalaA2.x, BalaA2.y + 10, 17, 0x0000);                       
          H_line(BalaA2.x, BalaA2.y + 9, 17, 0x0000);
          BalaA2.y = BalaA2.y - 2;
          if (BalaA2.y <= 0){                                               
            BalaA2.disparo = 2;
            BalaA2.desaparecer = 11;
          }
        }else if (BalaA2.disparo == 2){
          H_line(BalaA2.x, BalaA2.desaparecer, 17, 0x0000);
          if (BalaA2.desaparecer == 0){
            BalaA2.disparo = 3;
            BalaA2.recuperacion = 0;
            Serial2.println("b" + String(BalaA2.x) + "," + "2");
          }
          BalaA2.desaparecer--;
        }else if(BalaA2.disparo == 3){
          BalaA2.recuperacion++;
          if (BalaA2.recuperacion == 400){
            BalaA2.recuperacion = 220;
            BalaA2.disparo = 0;
            BalaA1.disparo = 0;
            TipoDisparo = 0;
            bandera_1_vez_bala12 = 0;
            Ataque2 = 0;
           }
         }           
       }
    }else if (TipoDisparo == 3){
      if (bandera_1_vez_bala123 == 0){
          BalaA3.disparo = 1;
        }
        if (digitalRead(PA_7) == HIGH){
          Ataque3 = 1;
          pushButton_Flag = 1;
        }
      if (Ataque3 == 1){
        if (bandera_1_vez_bala123 == 0){
          bandera_1_vez_bala123 = 1;
          BalaA3.y = Per_EjeY - 20;
          BalaA3.x = Per_EjeX+6;
        }
        if (BalaA3.disparo == 1){
          FillRect(BalaA3.x, BalaA3.y, 23, 10, 0x1681);
          H_line(BalaA3.x, BalaA3.y + 10, 23, 0x0000);
          H_line(BalaA3.x, BalaA3.y + 9, 23, 0x0000);
          BalaA3.y = BalaA3.y - 2;
          if (BalaA3.y <= 0){
            BalaA3.disparo = 2;
            BalaA3.desaparecer = 11;
          }
        }else if (BalaA3.disparo == 2){
          H_line(BalaA3.x, BalaA3.desaparecer, 23, 0x0000);
          if (BalaA3.desaparecer == 0){
            BalaA3.disparo = 3;
            BalaA3.recuperacion = 0;
            Serial2.println("b" + String(BalaA3.x) + "," + "3");
          }
          BalaA3.desaparecer--;
        }else if(BalaA3.disparo == 3){
          BalaA3.recuperacion++;
          if (BalaA3.recuperacion == 500){
            BalaA3.recuperacion = 220;
            BalaA2.disparo = 0;
            BalaA1.disparo = 0;
            BalaA3.disparo = 0;
            TipoDisparo = 0;
            bandera_1_vez_bala123 = 0;
            Ataque3 = 0;
           }
         }           
       }
    }else if (TipoDisparo == 4){
    
      if (bandera_1_vez_bala1234 == 0){
          BalaA4.disparo = 1;
        }
        if (digitalRead(PA_7) == HIGH){
          Ataque4 = 1;
          pushButton_Flag = 1;
        }
      if (Ataque4 == 1){
        if (bandera_1_vez_bala1234 == 0){
          bandera_1_vez_bala1234 = 1;
          BalaA4.y = Per_EjeY - 20;
          BalaA4.x = Per_EjeX+3;
        }
        if (BalaA4.disparo == 1){
          FillRect(BalaA4.x, BalaA4.y, 29, 10, 0x1681);
          H_line(BalaA4.x, BalaA4.y + 10, 29, 0x0000);
          H_line(BalaA4.x, BalaA4.y + 9, 29, 0x0000);
          H_line(BalaA4.x, BalaA4.y + 8, 29, 0x0000);
          BalaA4.y = BalaA4.y - 3;
          if (BalaA4.y <= 0){
            BalaA4.disparo = 2;
            BalaA4.desaparecer = 11;
          }
        }else if (BalaA4.disparo == 2){
          H_line(BalaA4.x, BalaA4.desaparecer, 29, 0x0000);
          if (BalaA4.desaparecer == 0){
            BalaA4.disparo = 3;
            BalaA4.recuperacion = 0;
            Serial2.println("b" + String(BalaA4.x) + "," + "4");
          }
          BalaA4.desaparecer--;
        }else if(BalaA4.disparo == 3){
          BalaA4.recuperacion++;
          if (BalaA4.recuperacion == 600){
            BalaA4.recuperacion = 220;
            BalaA2.disparo = 0;
            BalaA1.disparo = 0;
            BalaA3.disparo = 0;
            BalaA4.disparo = 0;
            TipoDisparo = 0;
            bandera_1_vez_bala1234 = 0;
            Ataque4 = 0;
           }
         }           
       }
    }

//***************************************************************************************************************************************
// IMPRESIÓN DE LAS BALAS EN PANTALLA
//***************************************************************************************************************************************
    if (BalaA4.disparo == 0 && BalaA3.disparo == 0 && BalaA2.disparo == 0 && BalaA1.disparo == 0 ){                     //Cuando se tienen 4 balas
      LCD_Bitmap(39, 221, 9, 19, Bala);
      LCD_Bitmap(26, 221, 9, 19, Bala);
      LCD_Bitmap(13, 221, 9, 19, Bala);
      LCD_Bitmap(0, 221, 9, 19, Bala);
    }else if (BalaA1.disparo == 0 && BalaA2.disparo == 0 && BalaA3.disparo == 0 && BalaA4.disparo != 0 || BalaA1.disparo == 0 && BalaA2.disparo == 0 && BalaA3.disparo != 0 && BalaA4.disparo == 0 || BalaA1.disparo == 0 && BalaA2.disparo != 0 && BalaA3.disparo == 0 && BalaA4.disparo == 0 || BalaA1.disparo != 0 && BalaA2.disparo == 0 && BalaA3.disparo == 0 && BalaA4.disparo == 0 ){          // Cuando se tienen 3 balas
      LCD_Bitmap(26, 221, 9, 19, Bala);
      LCD_Bitmap(13, 221, 9, 19, Bala);
      LCD_Bitmap(0, 221, 9, 19, Bala);
      FillRect(39, 221, 9, 19, 0x0000);
    }else if (BalaA1.disparo == 0 && BalaA2.disparo == 0 || BalaA1.disparo == 0 && BalaA3.disparo == 0 || BalaA1.disparo == 0 && BalaA4.disparo == 0 || BalaA2.disparo == 0 && BalaA3.disparo == 0 || BalaA2.disparo == 0 && BalaA4.disparo == 0 || BalaA3.disparo == 0 && BalaA4.disparo == 0){          //Se tienen 2 balas
      LCD_Bitmap(0, 221, 9, 19, Bala);
      LCD_Bitmap(13, 221, 9, 19, Bala);
      FillRect(26, 221, 9, 19, 0x0000);
      FillRect(39, 221, 9, 19, 0x0000);
    }else if (BalaA1.disparo == 0 && BalaA2.disparo != 0 && BalaA3.disparo != 0 && BalaA4.disparo != 0 || BalaA1.disparo != 0 && BalaA2.disparo == 0 && BalaA3.disparo != 0 && BalaA4.disparo != 0|| BalaA1.disparo != 0 && BalaA2.disparo != 0 && BalaA3.disparo == 0 && BalaA4.disparo != 0 || BalaA1.disparo != 0 && BalaA2.disparo != 0 && BalaA3.disparo != 0 && BalaA4.disparo == 0 ){        //Se tiene 1 bala
      LCD_Bitmap(0, 221, 9, 19, Bala);
      FillRect(13, 221, 9, 19, 0x0000);
      FillRect(26, 221, 9, 19, 0x0000);
      FillRect(39, 221, 9, 19, 0x0000);
    }else if (BalaA4.disparo != 0 && BalaA3.disparo != 0 && BalaA2.disparo != 0 && BalaA1.disparo != 0){                //Se tiene 0 balas
      FillRect(0, 221, 9, 19, 0x0000);
      FillRect(13, 221, 9, 19, 0x0000);
      FillRect(26, 221, 9, 19, 0x0000);
      FillRect(39, 221, 9, 19, 0x0000);
    }



  }//Finaliza el modo 1V1
  
  else if (modo == 2){
  }//Finaliza el modo PVE

  else if (modo == 3){
    if (bandera_1_vez_modo3 == 0){                      //para ejecutarse solo una vez
      ImagenRandom = random(0,4);                       //Se genera un valor random para seleccionar la imagen incial
      bandera_1_vez_modo3 = 1;
      BalaA1.recuperacion = 100;
      LCD_Print(text1, 95, 181, 2, 0xffff, 0x0000);     //Se coloca el mensaje de Perdiste
      digitalWrite(PC_5, LOW);
    }
    BalaA1.recuperacion++;
    if (BalaA1.recuperacion >= 100){                    //Luego de cierto tiempo se va rotando la imagen en orden
      ImagenRandom++;
      if (ImagenRandom > 4){
        ImagenRandom = 0;
      }
      if (ImagenRandom == 0){
        myFile = SD.open("quede.txt", FILE_READ);
        CargarFondo(123, 90, 115, 50, 0);
        myFile.close();
      }else if (ImagenRandom == 1){
        myFile = SD.open("planear.txt", FILE_READ);
        CargarFondo(123, 90, 115, 50, 0);
        myFile.close();
      }else if (ImagenRandom == 2){
        myFile = SD.open("esponja.txt", FILE_READ);
        CargarFondo(123, 90, 115, 50, 0);
        myFile.close();
      }else if (ImagenRandom == 3){
        myFile = SD.open("gato.txt", FILE_READ);
        CargarFondo(123, 90, 115, 50, 0);
        myFile.close();
      }else if (ImagenRandom == 4){
        myFile = SD.open("pinguino.txt", FILE_READ);
        CargarFondo(123, 90, 115, 50, 0);
        myFile.close();
      }
      BalaA1.recuperacion = 0;
    }
    
    if (pushButton_Flag == 0 && digitalRead(PA_7) == HIGH ){              //Si se presiona el botón se lleva al menu
      bandera_1_vez_modo3 = 0;
      digitalWrite(PC_4, HIGH);
      myFile = SD.open("Menu.txt", FILE_READ);
      CargarFondo(240, 320, 0, 0, 1);
      myFile.close();

      modo = 0;
      BalaA1.recuperacion = 220;
      pushButton_Flag = 1;
    }
    delay(50);
  }//Finaliza modo 3
  
  else if(modo == 4){
    if (bandera_1_vez_modo3 == 0){                      //para ejecutarse solo una vez
      ImagenRandom = random(0,4);                       //Se genera un valor random para seleccionar la imagen incial
      bandera_1_vez_modo3 = 1;
      BalaA1.recuperacion = 100;
      LCD_Print(text2, 104, 181, 2, 0xffff, 0x0000);     //Se coloca el mensaje de Ganaste
      digitalWrite(PC_5, LOW);
    }
    BalaA1.recuperacion++;
    if (BalaA1.recuperacion >= 100){                    //Luego de cierto tiempo se va rotando la imagen en orden
      ImagenRandom++;
      if (ImagenRandom > 4){
        ImagenRandom = 0;
      }
      if (ImagenRandom == 0){
        myFile = SD.open("copa.txt", FILE_READ);
        CargarFondo(123, 90, 115, 50, 0);
        myFile.close();
      }else if (ImagenRandom == 1){
        myFile = SD.open("listo.txt", FILE_READ);
        CargarFondo(123, 90, 115, 50, 0);
        myFile.close();
      }else if (ImagenRandom == 2){
        myFile = SD.open("mamado.txt", FILE_READ);
        CargarFondo(123, 90, 115, 50, 0);
        myFile.close();
      }else if (ImagenRandom == 3){
        myFile = SD.open("messi.txt", FILE_READ);
        CargarFondo(123, 90, 115, 50, 0);
        myFile.close();
      }else if (ImagenRandom == 4){
        myFile = SD.open("paz.txt", FILE_READ);
        CargarFondo(123, 90, 115, 50, 0);
        myFile.close();
      }
      BalaA1.recuperacion = 0;
    }
    
    if (pushButton_Flag == 0 && digitalRead(PA_7) == HIGH ){              //Si se presiona el botón se lleva al menu
      bandera_1_vez_modo3 = 0;
      digitalWrite(PC_4, HIGH);
      myFile = SD.open("Menu.txt", FILE_READ);
      CargarFondo(240, 320, 0, 0, 1);
      myFile.close();

      modo = 0;
      BalaA1.recuperacion = 220;
      pushButton_Flag = 1;
    }
    delay(50);
  }//Finaliza modo 4
  
}
//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void Sw_Flag(void){        //antirrebote
  if(digitalRead(PA_7) == LOW ){ // Boton 1
    pushButton_Flag = 0;
    if (bandera_1_vez == 0){
      TiempoInicial = millis();
      bandera_1_vez = 1;
    }
  }
}

char ConversionDecimal(char x, char y) {
int x1;
int y1;
int resultado;
x1 = ConversionASCII(x);
y1 = ConversionASCII(y);
resultado = int(x1)*16 + int(y1);  
return resultado;
}

char ConversionASCII(char x) {
  if (x == '0'){
    return 0;
  }else if (x == '1'){
    return 1;
  }else if (x == '2'){
    return 2;
  }else if (x == '3'){
    return 3;
  }else if (x == '4'){
    return 4;
  }else if (x == '5'){
    return 5;
  }else if (x == '6'){
    return 6;
  }else if (x == '7'){
    return 7;
  }else if (x == '8'){
    return 8;
  }else if (x == '9'){
    return 9;
  }else if (x == 'a'){
    return 10;
  }else if (x == 'b'){
    return 11;
  }else if (x == 'c'){
    return 12;
  }else if (x == 'd'){
    return 13;
  }else if (x == 'e'){
    return 14;
  }else if (x == 'f'){
    return 15;
  }else {
    return 0;
  }
}

void CargarFondo(int height, int width, int x1, int y1, int negro){
  if (negro == 1){
    LCD_Clear(0x00);
  }
  int UltimaPocicion = 0;
  int posicion_y = y1;
  int primerCero = 0;
  int char1 = 0;
  int char2 = 0;
  int x = 0;
  int x_seg_ciclo = 0;
   int totalBytes = myFile.size();
    if (myFile) {
      for (x_seg_ciclo = 0; x_seg_ciclo < height; x_seg_ciclo++){
        for (x = 0; x < width*2 ; x++){
          myFile.seek(UltimaPocicion);
          while (myFile.available()){
            char caracter = myFile.read();
            if (caracter == ','){
              UltimaPocicion = myFile.position();
              break;
            }else if (caracter == '0' && primerCero == 0){
              primerCero = 1;
            }else if (caracter == 'x'){
            
            }else if (caracter == ' '){
            
            }else if (caracter == 13){
             
            } else{
              primerCero = 0;
              char1 = caracter;
              char2 = myFile.read();
              MatrizFondo[x] = ConversionDecimal (char1, char2);
            }
          }
        }
        LCD_Bitmap(x1, posicion_y, width, 1, MatrizFondo);
        posicion_y++;
      }
    } else {
    // if the file didn't open, print an error:
    Serial.println("error opening archivo.txt");
  }
}


void Cargar_Sprite(int width, int height, int x, int y, int posicion, unsigned char bitmap[]){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = posicion;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}

void Cargar_Sprite_DifColor(int width, int height, int x, int y, int posicion, unsigned char bitmap[], char color1, char color2){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = posicion;
  unsigned char k1;
  unsigned char k2;
  unsigned char cambio_color;
  unsigned char cambio_color_2;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      k1 = bitmap[k];
      k2 = bitmap[k+1];
      if (k1 == 0){
        cambio_color = 0;
      }else{
        cambio_color = color1;
      }
      if (k2 == 0){
        cambio_color_2 = 0;
      }else{
        cambio_color_2 = color2;
      }
      
      LCD_DATA(cambio_color);
      LCD_DATA(cambio_color_2);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}


void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+w;
  y2 = y+h;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = w*h*2-1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c); 
      k = k - 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }
  
  char charInput ;
  int cLength = text.length();
  //Serial2.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    //Serial2.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
    for (int j = 0; j < height; j++){
        k = (j*(ancho) + index*width -1 - offset)*2;
        k = k+width*2;
       for (int i = 0; i < width; i++){
        LCD_DATA(bitmap[k]);
        LCD_DATA(bitmap[k+1]);
        k = k - 2;
       } 
    }
  }
  else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
    }
  }
  digitalWrite(LCD_CS, HIGH);
}
