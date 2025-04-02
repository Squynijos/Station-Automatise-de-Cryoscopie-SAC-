

//--------- INCLUDES ---------
#include "Adafruit_BME280.h"
#include "Definitions.h"
#include "ModbusRTUMaster.h"
 
//--------- OBJECTS ---------
Adafruit_BME280 bme;
 
HardwareSerial SerialSW(2);
HardwareSerial SerialRS485(3);

ModbusRTUMaster modbus(SerialRS485, P_DE);

//--------- VARIABLES ---------


//--------- MAIN PROG ---------
void setup() {
  Serial.begin(115200);
  
  // Init Objects
  initI2C();
  initRS485();

  //PIN Config
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  pinMode(LED_BUILTIN, OUTPUT);
  
}

//--------- LOOP DE DBG ---------
void loop() {
#if DEBUG
  readExtBME();
//LOOP uniquement utilisé pour le débug
  Serial.println("Temp:\t" + String(bme.readTemperature()));
  Serial.println("Hum:\t" + String(bme.readHumidity()));
  Serial.println("Press:\t" + String(bme.readPressure()));
 
  Serial.println();
  delay(1000);
#endif
}