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
#include <Decodeur.h>
#include "Definitions.h"
#include "ModbusRTUMaster.h" //1.0.5
#include <SD.h>
#include <SPI.h>
#include <Wire.h>


//---------- Configuration ----------

// Fonctionnalités
#define DEBUG       1
#define LOGGING     1
#define FILTRE_BAS  0


// Filters and delays
const unsigned long DELAY_M = 1000; // ms
bool firstMDone = false;
// SD card
String filename = "M"; // Define log file prefix here
const char* const HEADER = "NB,Lux\r\n";
const char* const FORMAT = "%lu,%.2f\r\n";


//---------- Objects ----------
// Modbus
HardwareSerial SerialRS485(Serial2);
ModbusRTUMaster modbus(SerialRS485, P_DE);
Decodeur decodeur(&Serial);



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


bool initSPI(){ //Fonctionnelle
  // Initialisation de la SD
  D(Serial.println("Initializing SD Card"));
  if(!SD.begin(P_CS_SD)){
    D(Serial.println("\t! Card Mount Failed"));
    return false;
  }

  // Verif avec type de carte
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE){
    D(Serial.println("\t! No SD card attached"));
    return false;
  }

  // Print le type et les détails de la carte
  if(DEBUG){
    Serial.print("\t- SD Card Type: ");
    if(cardType == CARD_MMC){
      Serial.println("MMC");
    } else if(cardType == CARD_SD){
      Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
      Serial.println("SDHC");
    } else {
      Serial.println("UNKNOWN");
    }

    // Information supplémentaire sur la carte
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("\t- SD Card Size: %lluMB\n", cardSize);
  }

  //Verif d'existance des différents fichiers, sinon création
  if(!SD.exists(DATA_FILE)){
    createCSV(DATA_FILE, HEADER);
  }

  return true;
}


bool createCSV(const char * path, const char * header){ //Fonctionnelle
  D(Serial.printf("Writing csv: %s\n", path));

  File file = SD.open("/ETA_Lux.csv", "w", true);
  if(!file){
    D(Serial.println("\t! Failed to open file for writing"));
    return false;
  }

  if(!file.print(String(header))){
    D(Serial.println("\t! Write failed"));
    file.close();
    return false;
  }

  file.close();
  return true;
}

bool logCSV(const char* path){ //Fonctionnelle
  D(Serial.printf("Appending to file: %s\n", path));
  
  File file = SD.open(path, FILE_APPEND);
  if(!file){
    D(Serial.println("\t! Failed to open file for appending"));
    return false;
  }

  if(!file.println(formatLog())){
    D(Serial.println("\t! Append failed"));
    file.close();
    return false;
  }

  file.close();
  return true;
}

String formatLog(){ //Fonctionnelle
  return (
    String(nlog) + "," +
    String(lumEXT) 
  );
}
//###################################################################


void readCommand() {
  String message = decodeur.getMessage();
  String command = decodeur.getCommandString();
  String arg = decodeur.getArgString(0);

  //Ack
  Serial.print("< "); Serial.println(message);
  command.toUpperCase();
  arg.toUpperCase();

  //----- STATUS -----
  if (command == "STATUS") {
    if (lastError) {
      Serial.print("> ERROR: ");
      Serial.println(lastError);
      clearError();
    } else if (firstMDone) {
      //We have a measure
      Serial.println("> READY");
    } else {
      //Still waiting on measure
      Serial.println("> INITIALIZING");
    }
  }

  //----- READ -----
  else if (command == "READ") {
    String comm, pos;
    if (arg.indexOf('.') != -1) {
      //split arg
      comm = arg.substring(0, arg.indexOf('.'));
      pos = arg.substring(arg.indexOf('.') + 1);
    } else {
      //Dont split
      comm = arg;
    }

    //Check arguments
    Serial.print("> ");
    if (comm.startsWith("L")) {
      Serial.println(lumEXT);
      nlog++;
    } else {
      Serial.print("! Argument not recognized: "); Serial.println(comm);
    }
  }

  //----- WRONG -----
  else {
    Serial.print("! Command not recognized: "); Serial.println(command);
  }
}



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

  initSPI();

}

void loop() {
  decodeur.refresh();
  //Read measures
  unsigned long elapsed = millis() - timer;
  if (elapsed >= DELAY_M) {
    runTime += elapsed / 1000; // not very accurate but good enough
    timer = millis();
    readLum();
  }

  if (decodeur.isAvailable()) {
    readCommand();
  }

}
