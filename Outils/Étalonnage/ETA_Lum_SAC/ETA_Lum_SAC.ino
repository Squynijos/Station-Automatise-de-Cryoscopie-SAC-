/*
 * Ce programme est un outil de test pour la station SAC
 * Il permet de vérifier les fonctionnalités du PCB principale à l'aide de commandes séries
 *
 * Auteur: Antoine Lepage
 * Date de dernière modif: 29 avril. 2025 par Antoine L.
 *
 *
 * Sources:
 *    Scan I2C: https://learn.adafruit.com/scanning-i2c-addresses/arduino
 *    Programme d'inspiration : ETA_Datalogger.ino
 */

#define VERSION "0.1.0"



//---------- Includes ----------
#include "Definitions.h"
#include "ModbusRTUMaster.h" //1.0.5
#include <SD.h>
#include <Wire.h>


//---------- Configuration ----------

// Fonctionnalités
#define DEBUG       1
#define LOGGING     1
#define FILTRE_BAS  0


// Filters and delays
const unsigned long DELAY_M = 1000; // ms

// SD card
String filename = "M"; // Define log file prefix here
const char* const HEADER = "NB,Sent,TempInt,TempExt,HumInt,HumExt\r\n";
const char* const FORMAT = "%lu,%lu,%.2f,%.2f,%.2f,%.2f\r\n";


//---------- Objects ----------
// Modbus
HardwareSerial SerialRS485(Serial2);
ModbusRTUMaster modbus(SerialRS485, P_DE);

File logfile;


//---------- Variables ----------

// Autres var
unsigned long nlog = 0;
unsigned long timer = 0, runTime = 0;

const char* lastError = NULL;
float lumEXT = 0;

//---------- Functions -----------
void reportSuccess(const char* message) {
  D(Serial.print("> "), Serial.println(message));
}

void reportSuccess(const String& message) {
  reportSuccess(message.c_str());
}

void reportError(const char* message) {
  if (message != lastError) {
    Serial.print("! ");
    Serial.println(lastError = message);
  }

}

void reportError(const String& message) {
  reportError(message.c_str());
}

void clearError() {
  lastError = NULL;
}


//      #######FONCTION SAC#######

void initPower(){ //Fonctionnelle
  pinMode(P_SHDN_12V, OUTPUT);  
  pinMode(P_SHDN_5V, OUTPUT);
  pinMode(P_SHDN_3V3, OUTPUT);  
}

//Pour activer les régulateurs/commutateurs
void enable12V()  {digitalWrite(P_SHDN_12V, HIGH);} //Fonctionnelle
void enable5V()   {digitalWrite(P_SHDN_5V, HIGH);} //Fonctionnelle
void enable3V3()  {digitalWrite(P_SHDN_3V3, HIGH);} //Fonctionnelle

void initRS485(){ //Fonctionnelle
  pinMode(P_RE, INPUT);
  pinMode(P_DE, OUTPUT);
  modbus.begin(9600, SERIAL_8N1, P_RX_485, P_TX_485);
  modbus.setTimeout(500);
}

void readLum(){ //Fonctionnelle
  uint16_t reg[1] = {0};
  modbus.readHoldingRegisters(ADDR_LUX, REG_LUM, reg, 1);
  lumEXT = reg[0];
}

//###################################################################




void logSD() {
  if (LOGGING && logfile) {
    D(Serial.printf(FORMAT, nlog, runTime, lumEXT));
    logfile.printf(FORMAT, nlog++, runTime, lumEXT);
  }
}


/**
 * Interprète et exécute les commandes provenant du port série
*/



//---------- Main Program ----------
void setup() {
  Serial.begin(115200);
  D(delay(1500));
  delay(500);

  //Wire.setClock(400000); // Set I2C clock speed to 400 kHz


  //Activate voltage regulators
  initPower();
  enable3V3();
  enable5V();
  enable12V();
  initRS485();

  


#if LOGGING
  //SD card config
  if (!SD.begin(P_CS_SD)) {
    reportError("Failed to init SD card");
    filename += "0.csv";
  }
  else {
    //Find next available filename
    for (int i = 0; i <= 999; i++) {
      String name = filename + String(i) + ".csv";
      if (!SD.exists(name)) {
        filename = name;
        break;
      }
    }

    //Create file and write header
    if (logfile = SD.open(filename, FILE_WRITE)) {
      reportSuccess("Writing header to " + filename);
      logfile.print(HEADER);
    } else {
      reportError("Failed to create file " + filename);
    }
  }
#endif
}

void loop() {

  //Read measures
  unsigned long elapsed = millis() - timer;
  if (elapsed >= DELAY_M) {
    runTime += elapsed / 1000; // not very accurate but good enough
    timer = millis();
    readLum();
    logSD();
  }

}
