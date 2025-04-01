#include "Adafruit_BME280.h"
#include "Sodaq_LSM303AGR.h"
 
#define RXD2 16
#define TXD2 17
 
#define GPS_BAUD 115200
 
Adafruit_BME280 bme;
Sodaq_LSM303AGR lsm303;
 
HardwareSerial gpsSerial(2);
int counter = 0;
 
void setup() {
  Serial.begin(115200);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  bme.begin(0x76);
  lsm303.enableMagnetometer();
 
  //gpsSerial.begin(GPS_BAUD, SERIAL_8N1, RXD2, TXD2);
 
}
 
void loop() {
  //gpsSerial.println(String(counter));
 
  Serial.println("Temp: " + String(bme.readTemperature()));
 
  // increment the counter
  counter++;
 
  delay(1000);
 
}