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
#include <Adafruit_BME280.h>
#include "Configs.h"
#include <Decodeur.h>
#include "Definitions.h"
#include <FS.h>
#include "ModbusRTUMaster.h" //1.0.5
#include <SD.h>
#include <SPI.h>
#include <TinyGPS++.h>
#include <Wire.h>


//---------- Configuration ----------

// Fonctionnalités
#define DEBUG       1
#define LOGGING     0
#define FILTRE_BAS  0


// Adresses
const uint8_t ADDR_TEMP_EXT =  1;
const uint8_t ADDR_BME =  0x77;

// Filters and delays
const unsigned long DELAY_M = 1000; // ms
constexpr float ALPHA_F = 1.0f; // range [0, 1]
static_assert(ALPHA_F >= 0 && ALPHA_F <= 1, "Valeur ALPHA_F invalide");

// SD card
String filename = "M"; // Define log file prefix here
const char* const HEADER = "runTime,TempInt,TempExt,HumInt,HumExt\r\n";
const char* const FORMAT = "%lu,%.2f,%.2f,%.2f,%.2f\r\n";




//---------- Objects ----------
Adafruit_BME280 bmeInt;
Decodeur decodeur(&Serial);

// Modbus
HardwareSerial SerialRS485(Serial2);
ModbusRTUMaster modbus(SerialRS485, P_DE);



// union Data {
//   // Structure de communication
//   struct Mesures {
//     float temp;
//     float hum;
//   } mesures;

//   uint8_t raw[sizeof(Mesures)];
// } data;


//---------- Variables ----------
// Valeurs filtrées, Last measured value
float tempInt, lmvTempInt;
float humInt, lmvHumInt;
float tempExt, lmvTempExt;
float humExt, lmvHumExt;
float luxInt, lmvLuxInt;

// Autres var
unsigned long nlog = 0;
unsigned long timer = 0, runTime = 0;
bool firstMDone = false;

const char* lastError = NULL;


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

void initI2C(){ //Fonctionnelle
  D(Serial.println("Initialisation I2C"));
    if(!bmeInt.begin(ADDR_BME_INT)){
      D(Serial.println("\t! Erreur avec BME"));
    }

}

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

void readBmeExt(){ //Fonctionnelle
  uint16_t reg[3] = {0, 0, 0};
  modbus.readHoldingRegisters(ADDR_BME_EXT, REG_HUM, reg, 3);
  humExt     = reg[0]/10.0;
  tempExt    = reg[1]/10.0;
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
    createCSV(DATA_FILE, "runTime,TempInt,TempExt,HumInt,HumExt\r\n");
  }

  return true;
}


bool createCSV(const char * path, const char * header){ //Fonctionnelle
  D(Serial.printf("Writing csv: %s\n", path));

  File file = SD.open("/ETA_BME.csv", "w", true);
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
    String(runTime) + "," +
    String(tempInt) + "," +
    String(tempExt) + "," +
    String(humInt) + "," +
    String(humExt)
  );
}
//###################################################################


/*
 * Ping the specified device
 * @param address The I2C address of the device
*/
void getMeasures() {
  tempInt = bmeInt.readTemperature();
  humInt = bmeInt.readHumidity();
  readBmeExt();

}


// void logSD() {
//   if (LOGGING && logfile) {
//     D(Serial.printf(FORMAT, nlog, runTime, tempInt, tempExt, humInt, humExt));
//     logfile.printf(FORMAT, nlog++, runTime, tempInt, tempExt, humInt, humExt);
//   }
// }


/**
 * Interprète et exécute les commandes provenant du port série
*/
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
    if (comm.startsWith("A")) {
      D(Serial.printf("%s> ", HEADER));
      //Serial.printf(FORMAT, nlog, runTime, tempInt, tempExt, humInt, humExt);
    } else if (comm.startsWith("T")) {
      Serial.println(pos == "EXT" ? tempExt : tempInt);
    } else if (comm.startsWith("H")) {
      Serial.println(pos == "EXT" ? humExt : humInt);
        if(pos == "EXT"){logCSV(DATA_FILE);} 
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

  //I2C Setup
  initI2C();
  
  initSPI();

}

void loop() {
  decodeur.refresh();

  //Read measures
  unsigned long elapsed = millis() - timer;
  if (elapsed >= DELAY_M) {
    runTime += elapsed / 1000; // not very accurate but good enough
    timer = millis();
    getMeasures();
    // float TInt = bmeInt.readTemperature();
    // TInt = 1 * TInt + 0;
    // float HInt = bmeInt.readHumidity();
    // HInt = 1 * HInt - 0;  //1.073 * HInt - 8.4292; //-6.20838E-5*pow(HInt,2) + 1.07983*HInt - 8.68641;     //-8.58801 + bmeInt.readHumidity() * 1.07799;
    
    // //Mesures EXT
    // if (getMeasures() <= 0) {
    //   reportError("Failed to read EXT sensors");
    // }

    // float TExt = data.mesures.temp;
    // TExt = 1 * TExt + 0;
    // float HExt = data.mesures.hum;
    // HExt = 1 * HExt - 0;  //1.073 * HExt - 8.4292; //-6.20838E-5*pow(HExt,2) + 1.07983*HExt - 8.68641;  //-8.58801 + data.mesures.hum * 1.07799;

    //D(Serial.printf("TInt=%+.2f, HInt=%+.2f, TExt=%+.2f, HExt=%+.2f\r\n", TInt, HInt, TExt, HExt));

    // #if FILTRE_BAS
    //   //Si c'est la première mesure on la met aussi dans la last mesure
    //   if (!firstMDone) { 
    //     lmvTempInt = TInt;
    //     lmvHumInt  = HInt;
    //     lmvTempExt = TExt;
    //     lmvHumExt  = HExt;
    //   }

    //   //Filtre passe-bas
    //   tempInt = ALPHA_F * TInt + (1-ALPHA_F) * lmvTempInt;
    //   humInt  = ALPHA_F * HInt + (1-ALPHA_F) * lmvHumInt;
    //   tempExt = ALPHA_F * TExt + (1-ALPHA_F) * lmvTempExt;
    //   humExt  = ALPHA_F * HExt + (1-ALPHA_F) * lmvHumExt;

    //   lmvTempInt = TInt;
    //   lmvHumInt  = HInt;
    //   lmvTempExt = TExt;
    //   lmvHumExt  = HExt;
    // #else
    //   //SECTION DE CODE AJOUTÉE PAR RICHARD M.
    //   tempInt = TInt;
    //   humInt = HInt;
    //   tempExt = TExt;
    //   humExt = HExt;
    // #endif

    firstMDone = true;
    //digitalWrite(PIN_LED_GREEN, !digitalRead(PIN_LED_GREEN));
  }

  if (decodeur.isAvailable()) {
    readCommand();
  }
}
