

//--------- INCLUDES ---------
#include "Definitions.h"
#include "ModbusRTUMaster.h"

#include "Adafruit_BME280.h"
#include "SD.h"
 
//--------- OBJECTS ---------
Adafruit_BME280 bme;
 
HardwareSerial SerialSW(2);
HardwareSerial SerialRS485(3);

ModbusRTUMaster modbus(SerialRS485, P_DE);

File logfile;

//--------- VARIABLES ---------
String filename = "M";

const char* const HEADER = "\r\n";// à faire
const char* const FORMAT = "\r\n";// à faire

//--------- MAIN PROG ---------
void setup() {
  Serial.begin(115200);
  
  // Init Objects
  initI2C();
  if(initSPI()){
    if(newFile("test")){
        D(Serial.println("création réussi"));
      }
      else{
        D(Serial.println("création échoué"));
    }
  }
  

  //PIN Config
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
}

//--------- LOOP DE DBG ---------
void loop() {
#if DEBUG
//LOOP uniquement utilisé pour le debug
  Serial.println("Temp:\t" + String(bme.readTemperature()));
  Serial.println("Hum:\t" + String(bme.readHumidity()));
  Serial.println("Press:\t" + String(bme.readPressure()));
 
  Serial.println();

  delay(1000);
#endif
}