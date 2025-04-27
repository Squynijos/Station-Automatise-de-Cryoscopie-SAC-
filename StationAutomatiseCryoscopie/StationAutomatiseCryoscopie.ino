/*------ TODO ------
  - Modbus - Changement adresse
  - GPS
  - SAT
  - Sleep
  - Lecture batterie
  - Gestion Erreur
  petite puce : logique du code
  autre puce : Thingsboard
*/

//--------- INCLUDES ---------
#include "Adafruit_BME280.h" //2.2.4
#include "ArduinoJson.h"
#include "Configs.h"
#include "Definitions.h"
#include <FS.h>
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

//--------- VARIABLES ---------
RTC_DATA_ATTR int bootCount = 0; // L'attribut RTC_DATA_ATTR indique que le variable est conserver en mémoire même entre les sleeps
Config config;

// Structure pour l'acquisition mesures
struct Mesures{
  int iteration;
  float timestamp;
  float longitude;
  float latitude;
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
  // float magX;
  // float magY;
  // float magZ;
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
  initSPI();

  if(!DEBUG){
    //Éteindre les sources d'alimentation
    //TODO

    // Configuration des périphériques à conserver en fonction
    //TODO

    // Mettre le esp32 en deep sleep
    // TODO
  }
  
}

//--------- LOOP DE DBG ---------
void loop() {
  //Read all values
  readVBat(data);
  readDirVent(data);
  readBmeExt(data);
  readLum(data);
  readBmeInt(data);
  readMagAccel(data);

  //Print result
  Serial.println("-------------------------------------");
  Serial.println("V Bat:\t\t"   + String(data.m.vBat));
  Serial.println("Dir Vent:\t"  + String(data.m.dirVent));
  Serial.println("Angle Vent:\t"+ String(data.m.angleVent));
  Serial.println("Vit Vent:\t"  + String(data.m.vitVent));
  Serial.println("Temp Ext:\t"  + String(data.m.tempExt));
  Serial.println("Hum Ext:\t"   + String(data.m.humExt));
  Serial.println("Press Ext:\t" + String(data.m.pressExt));
  Serial.println("Lum:\t\t"     + String(data.m.lum));

  Serial.println("Temp Int:\t"  + String(data.m.tempInt));
  Serial.println("Hum Int:\t"   + String(data.m.humInt));
  Serial.println("Press Int:\t" + String(data.m.pressInt));
  // Serial.println("MagX:\t\t"    + String(data.m.magX));
  // Serial.println("MagY:\t\t"    + String(data.m.magY));
  // Serial.println("MagZ:\t\t"    + String(data.m.magZ));
  Serial.println("AccelX:\t\t"  + String(data.m.accelX));
  Serial.println("AccelY:\t\t"  + String(data.m.accelY));
  Serial.println("AccelZ:\t\t"  + String(data.m.accelZ));

  Serial.println();
  createBin("/data.bin", data);
  DataStruct d;
  readBin("/data.bin", d);

  Serial.println("V Bat:\t\t"   + String(d.m.vBat));
  Serial.println("Dir Vent:\t"  + String(d.m.dirVent));
  Serial.println("Angle Vent:\t"+ String(d.m.angleVent));
  Serial.println("Vit Vent:\t"  + String(d.m.vitVent));
  Serial.println("Temp Ext:\t"  + String(d.m.tempExt));
  Serial.println("Hum Ext:\t"   + String(d.m.humExt));
  Serial.println("Press Ext:\t" + String(d.m.pressExt));
  Serial.println("Lum:\t\t"     + String(d.m.lum));

  Serial.println("Temp Int:\t"  + String(d.m.tempInt));
  Serial.println("Hum Int:\t"   + String(d.m.humInt));
  Serial.println("Press Int:\t" + String(d.m.pressInt));
  // Serial.println("MagX:\t\t"    + String(d.m.magX));
  // Serial.println("MagY:\t\t"    + String(d.m.magY));
  // Serial.println("MagZ:\t\t"    + String(d.m.magZ));
  Serial.println("AccelX:\t\t"  + String(d.m.accelX));
  Serial.println("AccelY:\t\t"  + String(d.m.accelY));
  Serial.println("AccelZ:\t\t"  + String(d.m.accelZ));

  //logCSV(DATA_FILE, data);

  Serial.println("-------------------------------------");


  // Serial.println();
  delay(1000);
}
