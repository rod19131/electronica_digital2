/*
Nombre: José Alejandro Rodríguez Porras 19131
Fecha: 29/09/2021
Laboratorio 7 SD
 */

#include <SPI.h>
#include <SD.h>
//__________________________variables______________________________________
File myFile;
File root;
int imagen;

//__________________________funciones______________________________________
void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
//__________________________setup______________________________________
void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  SPI.setModule(0);



  Serial.print("Initializing SD card...");
  //CS pin
  pinMode(PA_3, OUTPUT);

  if (!SD.begin(PA_3)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  root = SD.open("/");

  printDirectory(root, 0);

  Serial.println("done!");
  Serial.println("Elige una imagen: 1, 2, 3.");
  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.

  
  // re-open the file for reading:
}
//__________________________loop______________________________________
void loop()
{ if (Serial.available()){
  imagen = Serial.read(); // Reading Serial Data and saving in data variable
  // nothing happens after setup
  switch (imagen){
    case 49:
    myFile = SD.open("sdimage1.txt");
  if (myFile) {
    Serial.println("sdimage1.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
    Serial.println("lectura terminada");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  break;
  case 50:
    myFile = SD.open("sdimage2.txt");
  if (myFile) {
    Serial.println("sdimage2.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
    Serial.println("lectura terminada");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  break;
  case 51:
    myFile = SD.open("sdimage3.txt");
  if (myFile) {
    Serial.println("sdimage3.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
    Serial.println("lectura terminada");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
  break;  
  default:
     Serial.println("Elige una imagen: 1, 2, 3.");
  }
}}
