#include <SoftwareSerial.h>
 
#define SSerialRX        10  //Serial Receive pin
#define SSerialTX        11  //Serial Transmit pin
 
#define SSerialTxControl 3   // RS modul pin 3
 
 
// vytvorime seriovy port na pinu 10 a 11
SoftwareSerial RS485Serial(SSerialRX, SSerialTX); // RX, TX
 
int byteReceived;
 
void setup()  
{
  // Inicializujeme seriový port na klasickém pin 0 a 1 (TX a RX)
  Serial.begin(9600);  
 
  pinMode(SSerialTxControl, OUTPUT);  
  digitalWrite(SSerialTxControl, 0);  // Povolí přijímaní na modulu RS485
 
 
  RS485Serial.begin(14400); // rychlost RS485 musí být nastavená stejně jako na vysílači  
}
 
 
void loop()  {
 
   // když jsou nějaká data přijata na RS485
   if (RS485Serial.available())  
   {    
    byteReceived = RS485Serial.read();    // Přečte byte z modulu RS485
    Serial.write(byteReceived);           // a odešle je na Serial Monitor
   }  
     
 
}
