

//--------- INCLUDES ---------
#include "Adafruit_BME280.h"
#include "Definitions.h"
 
//--------- OBJECTS ---------
Adafruit_BME280 bme;
 
HardwareSerial SerialSW(2);

//--------- VARIABLES ---------


//--------- MAIN PROG ---------
void setup() {
  Serial.begin(115200);
  
  // Init Objects
  initI2C();

  //PIN Config
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

//--------- LOOP DE DBG ---------
void loop() {
#if DEBUG
//LOOP uniquement utilisé pour le débug
  Serial.println("Temp:\t" + String(bme.readTemperature()));
  Serial.println("Hum:\t" + String(bme.readHumidity()));
  Serial.println("Press:\t" + String(bme.readPressure()));
 
  Serial.println();
  delay(1000);
#endif
}