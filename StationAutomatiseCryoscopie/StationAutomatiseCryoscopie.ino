/*------ TODO ------
  - Magnétomètre (l'accéléromètre fonctionne for some reason)
  - Modbus
  - GPS
  - SAT
  - SD
  - Sleep
*/

//--------- INCLUDES ---------
#include "Adafruit_BME280.h"
#include <Adafruit_LSM303_Accel.h>
#include "Definitions.h"
#include <ModbusRTUMaster.h>
#include "Sodaq_LSM303AGR.h"
#include <Wire.h>
 
//--------- OBJECTS ---------
Adafruit_BME280 bme;
Sodaq_LSM303AGR lsm;

HardwareSerial SerialRS485(Serial2);
ModbusRTUMaster modbus(SerialRS485, P_DE);

//--------- VARIABLES ---------
struct Mesures{
  float angleVent;
  float dirVent;
  float vitVent;
  float tempExt;
  float humExt;
  float pressExt;
  float tempInt;
  float humInt;
  float pressInt;
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
  Serial.println("-------------------------------------");
  Serial.println("Temp:\t" + String(bme.readTemperature()));
  Serial.println("Hum:\t" + String(bme.readHumidity()));
  Serial.println("Press:\t" + String(bme.readPressure()));

  Serial.println("MagX:\t"+String(lsm.getMagX()));
  Serial.println("MagY:\t"+String(lsm.getMagY()));
  Serial.println("MagZ:\t"+String(lsm.getMagZ()));

  Serial.println("AccelX:\t"+String(lsm.getX()));
  Serial.println("AccelY:\t"+String(lsm.getY()));
  Serial.println("AccelZ:\t"+String(lsm.getZ()));
  Serial.println("-------------------------------------");
  readvent();
  Serial.println();
  delay(1000);
}