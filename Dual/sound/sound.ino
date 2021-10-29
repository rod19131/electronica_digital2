int buzzerPin = 40;

void spaceGun(int maximum){
  for(int i = 0; i < maximum; i++){
    digitalWrite(buzzerPin, HIGH);
    delay(i);
    digitalWrite(buzzerPin, LOW);
    delay(i);
  }
}

void randSound(int maximum){
  tone(buzzerPin, random(maximum, 10*maximum));
  delay(maximum);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly: 
  spaceGun(20);
  delay(1000);
}
