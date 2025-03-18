//---------- INCLUDES ----------

#include <Arduino.h>
#include "pin_config.h"
#include "Adafruit_BME280.h"
#include "Sodaq_LSM303AGR.h"

//---------- ADRESSES ---------

#define ADDR_BME 0x76
#define ADDR_LSM 0x1E

//---------- OBJETS ---------

Adafruit_BME280 bme280;
Sodaq_LSM303AGR lsm303;

//---------- MAIN PROGRAM ---------
void setup()
{
  // configurePins();

  // Initialisation
  Serial.begin(115200);
  delay(500);

  bme280.begin(ADDR_BME);
  lsm303.disableMagnetometer();

  // Logique
  // Serial.println(bme280.readTemperature());
}

void loop()
{
  Serial.println(bme280.readTemperature());
  Serial.println(bme280.readPressure());
  Serial.println(bme280.readHumidity());
  Serial.println();
  delay(500);
  // No-Op
}