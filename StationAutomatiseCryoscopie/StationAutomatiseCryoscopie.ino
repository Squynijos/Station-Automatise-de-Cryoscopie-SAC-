/*------ TODO ------
  - Modbus - Changement adresse
  - GPS
  - SAT
  - SD
  - Sleep
  - Gestion Erreur
*/

//--------- INCLUDES ---------
#include "Adafruit_BME280.h" //2.2.4
#include "Definitions.h"
#include "ModbusRTUMaster.h" //1.0.5
#include <SD.h>
#include "Sodaq_LSM303AGR.h" //2.0.1
#include <SPI.h>
#include <Wire.h>
 
//--------- OBJECTS ---------
Adafruit_BME280 bme;
Sodaq_LSM303AGR lsm;

HardwareSerial SerialRS485(Serial2);
ModbusRTUMaster modbus(SerialRS485, P_DE);

//--------- Constants ---------
const String dataFile = "data.csv";

//--------- VARIABLES ---------
struct Mesures{
  float vBat;
  float angleVent;
  float dirVent;
  float vitVent;
  float tempExt;
  float humExt;
  float pressExt;
  float tempInt;
  float humInt;
  float pressInt;
  float lum;
  float accelX;
  float accelY;
  float accelZ;
  float magX;
  float magY;
  float magZ;
};

union DataStruct{
  Mesures m;
  uint8_t raw[sizeof(Mesures)];
};
DataStruct data;

//--------- MAIN PROG ---------
void setup() {
  Serial.begin(115200);
  delay(500);
  
  //Power
  initPower();
  enable3V3();
  enable12V();

  // Init Objects
  initI2C();
  initRS485();
}

//--------- LOOP DE DBG ---------
void loop() {
  //Read all values
  readDirVent(data);
  readBmeExt(data);
  readLum(data);
  readBmeInt(data);
  readMagAccel(data);

  //Print result
  Serial.println("-------------------------------------");
  Serial.println("Dir Vent:\t"+String(data.m.dirVent));
  Serial.println("Angle Vent:\t"+String(data.m.angleVent));
  Serial.println("Vit Vent:\t"+String(data.m.vitVent));
  Serial.println("Temp Ext:\t"+String(data.m.tempExt));
  Serial.println("Hum Ext:\t"+String(data.m.humExt));
  Serial.println("Press Ext:\t"+String(data.m.pressExt));
  Serial.println("Lum:\t\t"+String(data.m.lum));

  Serial.println("Temp Int:\t"+String(data.m.tempInt));
  Serial.println("Hum Int:\t"+String(data.m.humInt));
  Serial.println("Press Int:\t"+String(data.m.pressInt));
  Serial.println("MagX:\t\t"+String(data.m.magX));
  Serial.println("MagY:\t\t"+String(data.m.magY));
  Serial.println("MagZ:\t\t"+String(data.m.magZ));
  Serial.println("AccelX:\t\t"+String(data.m.accelX));
  Serial.println("AccelY:\t\t"+String(data.m.accelY));
  Serial.println("AccelZ:\t\t"+String(data.m.accelZ));
  Serial.println("-------------------------------------");

  logSD(dataFile, "Allo");

  Serial.println();
  delay(1000);
}