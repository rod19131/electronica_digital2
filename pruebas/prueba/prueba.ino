#include <SPI.h>
#include <SD.h>

File myFile;
void printimage();
int imagen;

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  SPI.setModule(0);

  Serial.print("Initializing SD card...");
  pinMode(PA_3, OUTPUT);

  if (!SD.begin(PA_3)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  Serial.println();
  Serial.println("Eliga una imagen con 1, 2 o 3: "); 
 
}

void printimage(){
  //
  switch (imagen){
    case 1: 
    myFile = SD.open("froggo.txt");
    if (myFile) {
      Serial.println("froggo.txt: ");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
    } else {
    // if the file didn't open, print an error:
    Serial.println("error opening .txt");
    }
    break;
    case 2: 
    myFile = SD.open("llama.txt");
    if (myFile) {
      Serial.println("llama.txt: ");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
    } else {
    // if the file didn't open, print an error:
    Serial.println("error opening .txt");
    }
    break;
    case 3: 
    myFile = SD.open("jake.txt");
    if (myFile) {
      Serial.println("jake.txt: ");
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
    } else {
    // if the file didn't open, print an error:
    Serial.println("error opening .txt");
    }
    break;
    default: 
     Serial.println("No existe");
     break;
  }

  Serial.println();
  Serial.println();
}

void loop()
{
  if(Serial.available()) // Check for availablity of data at Serial Port
  {
    imagen = Serial.read(); // Reading Serial Data and saving in data variable
    void printimage();
    Serial.println("Eliga una imagen con 1, 2 o 3: "); 
  }


}
