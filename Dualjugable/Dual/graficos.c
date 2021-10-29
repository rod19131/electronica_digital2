/*
 * Convertidor de imagen a hex R5G6B5 online: https://lvgl.io/tools/imageconverter
 * 1. Cargar imagen de 320x240px
 * 2. Color format: True color
 * 3. Output format: C array
 * 4. Extraer values del array LV_COLOR_DEPTH == 16 && LV_COLOR_16_SWAP != 0
 * 5. Crear variable: const unsigned char imgName [] PROGMEM ={values};
 */
#include <avr/pgmspace.h> 
 char MatrizFondo [640] PROGMEM ={};
