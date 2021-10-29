/*
Electrónica Digital 2 
Lab 6
Alejandro Rodríguez
*/
//_____________________Variables_________________________________
int inicio, jug1, jug2, cont1, cont2 = 0;
void setup() {
  pinMode(PUSH1, INPUT_PULLUP); 
  pinMode(PUSH2, INPUT_PULLUP); 
  Serial1.begin(9600); 
}
//_____________________Loop______________________________________
void loop() 
{ Serial1.println(cont1); 
  Serial1.write(cont1);
  if ((digitalRead(PUSH1) == LOW || digitalRead(PUSH2) == LOW) && inicio == 0){sem();}
  if (inicio == 1){
    if (digitalRead(PUSH1) == LOW){jug1 = 1;}
    if (digitalRead(PUSH2) == LOW){jug2 = 1;}
    if (digitalRead(PUSH1) == HIGH && jug1 == 1){
      jug1 = 0;
      cont1++;}
    if (digitalRead(PUSH2) == HIGH && jug2 == 1){
      jug2 = 0;
      cont2++;}
    switch (cont1){
      case 0:
        analogWrite(GREEN_LED, 0);
        break;
      case 1:
        analogWrite(GREEN_LED, 32);
        break;
      case 2:
        analogWrite(GREEN_LED, 64);
        break;
      case 3:
        analogWrite(GREEN_LED, 96);
        break;
      case 4:
        analogWrite(GREEN_LED, 128);
        break;
      case 5:
        analogWrite(GREEN_LED, 160);
        break;
      case 6:
        analogWrite(GREEN_LED, 192);
        break; 
      case 7:
        analogWrite(GREEN_LED, 224);
        break; 
      case 8:
        analogWrite(RED_LED, 255);
        analogWrite(BLUE_LED, 255);
        analogWrite(GREEN_LED, 255);
        delay(2000);
        analogWrite(RED_LED, 0);
        analogWrite(BLUE_LED, 0);
        analogWrite(GREEN_LED, 255);
        inicio = 0;
        cont1 = 0;
        cont2 = 0;
        break; 
    }
    switch (cont2){
      case 0:
        analogWrite(BLUE_LED, 0);
        break;
      case 1:
        analogWrite(BLUE_LED, 32);
        break;
      case 2:
        analogWrite(BLUE_LED, 64);
        break;
      case 3:
        analogWrite(BLUE_LED, 96);
        delay(10);
        break;
      case 4:
        analogWrite(BLUE_LED, 128);
        break;
      case 5:
        analogWrite(BLUE_LED, 160);
        break;
      case 6:
        analogWrite(BLUE_LED, 192);
        break; 
      case 7:
        analogWrite(BLUE_LED, 224);
        break; 
      case 8:
        analogWrite(RED_LED, 255);
        analogWrite(BLUE_LED, 255);
        analogWrite(GREEN_LED, 255);
        delay(2000);
        analogWrite(RED_LED, 0);
        analogWrite(BLUE_LED, 255);
        analogWrite(GREEN_LED, 0);
        inicio = 0;
        cont1 = 0;
        cont2 = 0;
        break; 
    }

  }
}
void sem(void){
  analogWrite(RED_LED, 255);
  analogWrite(BLUE_LED, 0);
  analogWrite(GREEN_LED, 0);
  delay(1000);
  analogWrite(RED_LED, 255);
  analogWrite(BLUE_LED, 0);
  analogWrite(GREEN_LED, 255);
  delay(1000);
  analogWrite(RED_LED, 0);
  analogWrite(BLUE_LED, 0);
  analogWrite(GREEN_LED, 255);
  delay(1000);
  analogWrite(RED_LED, 0);
  analogWrite(BLUE_LED, 0);
  analogWrite(GREEN_LED, 0);
  inicio = 1;    
}

// ---------------------------------------------------------------------------------------
