/*
  Multple serial test
 
  Receives from the main serial port, sends to the others. 
  Receives from serial port 1, sends to the main serial (Serial 0).
 
  The circuit: 
  * Any serial device attached to Serial port 1
  * Serial monitor open on Serial port 0:
 
  created 30 Dec. 2008
  by Tom Igoe
 
  This example code is in the public domain.
 
*/
int jug1, cont1, inByte = 0;
void setup() {
  pinMode(PUSH1, INPUT_PULLUP); 
  // initialize both serial ports:
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  inByte = Serial1.read();
  delay(500);
  Serial.println(inByte);
  // read from port 1, send to port 0:
/*  if (digitalRead(PUSH1) == LOW){jug1 = 1;}
    if (digitalRead(PUSH1) == HIGH && jug1 == 1){
      jug1 = 0;
      cont1++;}*/
}
