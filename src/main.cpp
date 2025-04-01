//---------- INCLUDES ----------

#include <Arduino.h>
#include "Adafruit_BME280.h"
#include "Sodaq_LSM303AGR.h"

//---------- ADRESSES ---------

#define ADDR_BME 0x76
#define ADDR_LSM 0x1E

//---------- OBJETS ---------

Adafruit_BME280 bme280;
Sodaq_LSM303AGR lsm303;

//---------- FONCTIONS ----------

//---------- MAIN PROGRAM ---------
void setup()
{
  // Initialisation
  Serial.begin(115200);
  delay(500);
  bme280.begin(ADDR_BME);
  lsm303.disableAccelerometer();
  lsm303.enableMagnetometer();

  // PINS Config
  // pinMode(DEL_B, OUTPUT);
  // digitalWrite(DEL_B, HIGH);
}

void loop()
{
  // No-Op
}