int inByte = '1';
void setup() {
  // initialize both serial ports:
  Serial4.begin(9600);
}

void loop() {
  // read from port 1, send to port 0:
  if (Serial4.available()) {
    //inByte = Serial1.read();
  }
  Serial4.print("1");
  delay(500);
}
