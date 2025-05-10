#pragma once

//---------- Broches ------------
  //UART
#define P_S1 26
#define P_S0 25
#define P_RX_SW 16
#define P_TX_SW 17
#define P_TX_485 4
#define P_RX_485 12

  //I2C
#define P_SCL 22
#define P_SDA 21
  //SPI
#define P_MISO 19
#define P_MOSI 23
#define P_SCK 18
#define P_CS_SD 10
  //PIN ANALOG
#define P_35 35
#define P_34 34
#define P_39 39
  //DEL RGB
#define P_DEL_B 8
#define P_DEL_R 7
#define P_DEL_G 6
  //ON-OFF
#define P_SHDN_12V 2
#define P_SHDN_5V 5
#define P_SHDN_3V3 13
#define P_SAT 14
  //RS485
#define P_DE 9
#define P_RE 27
  //AUTRES
#define P_3DFIX 0
#define P_VBAT 15

//---------- Adresses -----------
#define ADDR_BME_INT 0x76
#define ADDR_ANEMO 3
#define ADDR_GIROU 2
#define ADDR_BME_EXT 1
#define ADDR_LUX 5
#define ADDR_ACCEL 0x19
#define ADDR_MAG 0x1E

//---------- Registres ----------
#define REG_VIT   0
#define REG_DIR   1
#define REG_ANGLE 0
#define REG_TEMP  1
#define REG_PRESS 2
#define REG_HUM   0
#define REG_LUM   1

//---------- Flags --------------
#define DEBUG true
#define SLEEP_EN false

//---------- Configs ------------
#define BAUD_DEBUG 115200

//---------- PATHS SD -----------
#define HEADER "Timestamp, Longitude, Latitude, HDOP, VBat, TemperatureInt, PressionInt, HumiditeInt, AccelX, AccelY, AccelZ, TemperatureExt, PressionExt, HumiditeExt, VitesseVent, DirectionVent, AngleVent, Luminosite\n"
#define DATA_FILE "/data.csv"
#define CONFIG_FILE "/config.json"

//---------- MACROS -------------
#if DEBUG
  #define D(...) (__VA_ARGS__)
#else
  #define D(...) (void(0))
#endif



#include "IridiumSBD.h"

HardwareSerial SerialSatGps(Serial1);
IridiumSBD modemSat(SerialSatGps, P_SAT);

void initPower(){ //Fonctionnelle
  pinMode(P_SHDN_12V, OUTPUT);  
  pinMode(P_SHDN_5V, OUTPUT);
  pinMode(P_SHDN_3V3, OUTPUT);  
}

//Pour activer les régulateurs/commutateurs
void enable12V()  {digitalWrite(P_SHDN_12V, HIGH);} //Fonctionnelle
void enable5V()   {digitalWrite(P_SHDN_5V, HIGH);} //Fonctionnelle
void enable3V3()  {digitalWrite(P_SHDN_3V3, HIGH);} //Fonctionnelle

//Pour désactiver les tensions
void disable12V() {digitalWrite(P_SHDN_12V, LOW);}
void disable5V()  {digitalWrite(P_SHDN_5V, LOW);}
void disable3V3() {digitalWrite(P_SHDN_3V3, LOW);}

void initUART(){ //À TESTER
  //Les pins pour le select
  pinMode(P_S0, OUTPUT);
  pinMode(P_S1, OUTPUT);
  digitalWrite(P_S0, HIGH);
  digitalWrite(P_S1, LOW);

  pinMode(P_SAT, OUTPUT);
  digitalWrite(P_SAT, HIGH);

  SerialSatGps.begin(19200, SERIAL_8N1, P_TX_SW, P_RX_SW);

  //Configure Satellite
  D(Serial.println("Configurating Modem"));
  modemSat.setPowerProfile(IridiumSBD::DEFAULT_POWER_PROFILE);     // Assume battery power (USB power: IridiumSBD::USB_POWER_PROFILE)
  modemSat.adjustSendReceiveTimeout(240);           // Timeout for Iridium send/receive commands (default = 300 s)
  modemSat.adjustATTimeout(240 / 2);           // Timeout for Iridium startup (default = 240 s)
}


void setup() {
  initPower();
  enable3V3();
  enable5V();
  enable12V();

  initUART();

  Serial.begin(115200);
  delay(100);

}

void loop() {
  SerialSatGps.print("AT\r");
  SerialSatGps.flush();
  while(Serial.available()){
    char a = Serial.read();
    SerialSatGps.print(a);
    //Serial.print(a);
  }

  while(SerialSatGps.available()){
    char a = SerialSatGps.read();
    Serial.print(a);
  }
  delay(5000);
}
