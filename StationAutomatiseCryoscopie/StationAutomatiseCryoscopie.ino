/*
  Nom de programme : StationAutomiseCryoscopie
  Auteurs : Liam Lebrun et Antoine Lepage

  Version IDE         : 2.3.6
  Version Board ESP32 : 2.0.17
*/


/*------ TODO ------
  - Modbus - Changement adresse - tester le retry
  - SAT
  - Lecture batterie - à étalonner
  - Gestion Erreur
  petite puce : logique du code
  autre puce : Thingsboard
  puce cool: faire fonctionner si la carte sd n'est pas mount
  plus de puce: envoyer err par sat
  - Conserver dernière longitude/latitude pour avoir un fallback
*/

/*
  1. Verifier le passage du UART sur port SAT
  2. Tester le begin du modem
  3. Test transmission bidon
  4. Trouver le TB
  5. Vérife de récupération
*/

//--------- INCLUDES ---------
#include "Adafruit_BME280.h" //2.2.4
#include "Adafruit_GPS.h"    //1.7.5
#include "ArduinoJson.h"
#include "Configs.h"
#include "Definitions.h"
#include <ESP32Time.h>
#include <FS.h>
#include "IridiumSBD.h"      // https://github.com/sparkfun/SparkFun_IridiumSBD_I2C_Arduino_Library (v3.0.6)
#include "ModbusRTUMaster.h" //1.0.5
#include <SD.h>
#include "Sodaq_LSM303AGR.h" //2.0.1
#include <SPI.h>
#include "TimeLib.h"         // https://github.com/PaulStoffregen/Time (v1.6.1)
#include <Wire.h>
 
//--------- OBJECTS ---------
// Capteurs internes
Adafruit_BME280 bme;
Sodaq_LSM303AGR lsm;

// UART
HardwareSerial SerialSatGps(Serial1);
IridiumSBD modemSat(SerialSatGps, P_SAT);
Adafruit_GPS gps(&SerialSatGps);

ESP32Time rtc(-5*60*3600);

// Modbus
HardwareSerial SerialRS485(Serial2);
ModbusRTUMaster modbus(SerialRS485, P_DE);

//--------- Constants ---------
const uint8_t BAT_CUT_OFF = 11; //V

//--------- VARIABLES ---------
RTC_DATA_ATTR int bootCount  = 0; // L'attribut RTC_DATA_ATTR indique que le variable est conserver en mémoire même entre les sleeps
RTC_DATA_ATTR bool firstBoot = true;
unsigned long unixtime       = 0; 
Config config;

// Union to store Iridium Short Burst Data (SBD) Mobile Originated (MO) messages
// Même que Glacier SMA pour compatibilité avec le TB
typedef union
{
  struct
  {
    uint32_t  unixtime;           // UNIX Epoch time                (4 bytes)
    int16_t   temperatureInt;     // Internal temperature (°C)      (2 bytes)   * 100
    uint16_t  humidityInt;        // Internal humidity (%)          (2 bytes)   * 100
    uint16_t  pressureExt;        // External pressure (hPa)        (2 bytes)   - 400 * 100
    int16_t   temperatureExt;     // External temperature (°C)      (2 bytes)   * 100
    uint16_t  humidityExt;        // External humidity (%)          (2 bytes)   * 100
    int16_t   pitch;              // Pitch (°)                      (2 bytes)   * 100
    int16_t   roll;               // Roll (°)                       (2 bytes)   * 100
    uint32_t  solar;              // Solar illuminance (lx)         (4 bytes)   * 10000
    uint16_t  windSpeed;          // Mean wind speed (m/s)          (2 bytes)   * 100
    uint16_t  windDirection;      // Mean wind direction (°)        (2 bytes)	  * 10
    uint16_t  windGustSpeed;      // Wind gust speed (m/s)          (2 bytes)   * 100
    uint16_t  windGustDirection;  // Wind gust direction (°)        (2 bytes)	  * 10
    int32_t   latitude;           // Latitude (DD)                  (4 bytes)   * 1000000
    int32_t   longitude;          // Longitude (DD)                 (4 bytes)   * 1000000
    uint8_t   satellites;         // # of satellites                (1 byte)
    uint16_t  hauteurNeige;       // Hauteur de neige (mm)          (2 bytes)   * 1
    uint16_t  voltage;            // Battery voltage (V)            (2 bytes)   * 100
    uint16_t  transmitDuration;   // Previous transmission duration (2 bytes)
    uint8_t   transmitStatus;     // Iridium return code            (1 byte)
    uint16_t  iterationCounter;   // Message counter                (2 bytes)
  } __attribute__((packed));                              // Total: (48 bytes)
  uint8_t bytes[48];
} MSG_SAT;

// Structure pour l'acquisition mesures
struct Mesures{
  int iteration;
  uint32_t timestamp;
  float longitude;
  float latitude;
  float hdop;
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
  D(Serial.begin(115200));
  if(DEBUG){
    Serial.begin(115200);
    delay(500);

    //On active tout
    if(SLEEP_EN){
      wakeup();
    }

    initPower();
    enable3V3();
    enable5V();
    enable12V();

    initSPI();

    initI2C();
    initRS485();
    initRTC();
    initUART();

    //On skip le reste du setup
    return;
    Serial.println("Should not print");
  }
  
  //### NON DEBUG CODE ###
  readVBat(data);
  if(data.m.vBat < BAT_CUT_OFF){
    D(Serial.println("! Battery to low"));
    bootCount++;
    goToSleep(30); //3600 / config.acquisitionParHeure
  }
  wakeup();

  //Power on internal devices and configurations
  initPower();
  enable3V3();

  //Init SPI et Read config file, doit être fait en premier pour obtenir les configs
  initSPI();

  //Initialise le reste des communications
  initI2C();
  initRS485();
  initRTC();
  initUART();

  //Get internal sensors values
  readBmeInt(data);
  readMagAccel(data);

  //Get external sensors values
  enable12V();
  readDirVent(data);
  readVitVent(data);
  readBmeExt(data);
  readLum(data);
  disable12V();

  //Read GPS et sync RTC
  //readGPS(data);

  //Save data as bin on SD
  int envoie = bootCount % ((24 / config.sat.transmissionParJour)*config.acquisitionParHeure);
  char binName[12];
  sprintf(binName, "/binary/%02d.bin", envoie);
  createBin(binName, data);

  //Moyenne des données après X iterations déterminé par le nombre d'acquisition/heure et le nombre de transmission/jour
  if(bootCount % ((24 / config.sat.transmissionParJour)*config.acquisitionParHeure) == 0){
    D(Serial.println("Starting sending process..."));

    //Moyenne du data
    DataStruct avData;
    moyenneBin(avData);
    
    //Logging data
    logCSV(DATA_FILE, avData);

    //Sending data
    if(SEND_SAT){
      sendSAT(avData);
    }
  }

  bootCount++;

  //Sleep
  deinitSPI();
  goToSleep(30);  //3600 / config.acquisitionParHeure
}

//--------- LOOP DE DBG ---------
void loop() {

  //Read all values
  Serial.println("---------------- Start --------------");
  Serial.println("> Reading values...");
  data.m.iteration = bootCount;

  //Read values
  readVBat(data);
  readDirVent(data);
  readVitVent(data);
  readBmeExt(data);
  readLum(data);
  readBmeInt(data);
  readMagAccel(data);
  //readGPS(data);
  readRTC(data);

  //Print result
  Serial.println("------------- Values #" + String(bootCount) + " -------------");
  Serial.println("iteration:\t" + String(data.m.iteration));

  Serial.println("timestamp:\t" + String(data.m.timestamp));
  Serial.println("longitude:\t" + String(data.m.longitude));
  Serial.println("latitude:\t"  + String(data.m.latitude));
  Serial.println("hdop:\t\t"    + String(data.m.hdop));

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

  logCSV(DATA_FILE, data);

  char binName[12];
  sprintf(binName, "/binary/%02d.bin", bootCount);
  createBin(binName, data);

  Serial.println("Checking content of binary");
  File dir = SD.open("/binary");
  while(true){
    File file = dir.openNextFile();
    if(!file){
      break;
    }
    const char* fileName = file.name();
    Serial.println(fileName);
  }

  if(SEND_SAT){
    sendSAT(data);
  }
  
  Serial.println("-------------------------------------");
  Serial.println();
  Serial.println();

  bootCount++;
  if(SLEEP_EN){
    deinitSPI();
    disable3V3();
    disable12V();
    goToSleep(30);
  }
  delay(20000);
}
