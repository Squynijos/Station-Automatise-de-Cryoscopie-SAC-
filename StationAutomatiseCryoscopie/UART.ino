// //Pour le communication avec le gps et le SAT

void initUART(){ //À TESTER
  //Les pins pour le select
  pinMode(P_S0, OUTPUT);
  pinMode(P_S1, OUTPUT);
  digitalWrite(P_S0, LOW);
  digitalWrite(P_S1, LOW);

  pinMode(P_SAT, OUTPUT);
  digitalWrite(P_SAT, LOW);

  //Default to gps
  D(Serial.println("Configurating GPS"));
  SerialSatGps.begin(config.gps.baud, SERIAL_8N1, P_RX_SW, P_TX_SW);
  while(!SerialSatGps){
    D(Serial.print("."));
  }
  SerialSatGps.println(PMTK_SET_NMEA_OUTPUT_OFF);//gps.sendCommand(PMTK_SET_NMEA_OUTPUT_OFF); //Use RMC and GGA
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); //Update rate = 1Hz
  D(Serial.println("\t> Done"));

  //Configure Satellite
  D(Serial.println("Configurating Modem"));
  modemSat.setPowerProfile(IridiumSBD::DEFAULT_POWER_PROFILE);     // Assume battery power (USB power: IridiumSBD::USB_POWER_PROFILE)
  modemSat.adjustSendReceiveTimeout(config.sat.timeout);           // Timeout for Iridium send/receive commands (default = 300 s)
  modemSat.adjustATTimeout(config.sat.timeout / 8);           // Timeout for Iridium startup (default = 240 s)
}

bool readGPS(DataStruct &ds){ //Fonctionnel
  D(Serial.println("Communication avec gps"));

  //Active ls switch sur le bon appareil
  D(Serial.println("\t- Selecting Port"));
  digitalWrite(P_S0, LOW);
  digitalWrite(P_S1, LOW);
  //SerialSatGps.begin(config.gps.baud, SERIAL_8N1, P_RX_SW, P_TX_SW);

  //attente d'un message
  D(Serial.println("\t- Waiting for NMEA"));
  /*
    REF: https://tavotech.com/gps-nmea-sentence-structure/
    Il faut attendre plusieurs messages 
    car la date et l'heure ne sont pas dans le même message
  */
  unsigned long startTime = millis();
  while(true) {

    //Timeout
    if(millis() - startTime > config.gps.timeout*1000){
      D(Serial.println("\t! GPS Timeout"));
      return false;
    }

    char c = gps.read();
      
    //On décode le message recu
    if(gps.newNMEAreceived()){
      char* s = gps.lastNMEA();
      D(Serial.print("\t- Last NMEA: " + String(s)));
      if(!gps.parse(s)){
        D(Serial.println("\t! Parsing error"));
        continue;
      }
      if(PRINT_DEBUG){
        Serial.print("\nTime: ");
        if (gps.hour < 10) { Serial.print('0'); }
        Serial.print(gps.hour, DEC); Serial.print(':');
        if (gps.minute < 10) { Serial.print('0'); }
        Serial.print(gps.minute, DEC); Serial.print(':');
        if (gps.seconds < 10) { Serial.print('0'); }
        Serial.print(gps.seconds, DEC); Serial.print('.');
        if (gps.milliseconds < 10) {
          Serial.print("00");
        } else if (gps.milliseconds > 9 && gps.milliseconds < 100) {
          Serial.print("0");
        }
        Serial.println(gps.milliseconds);
        Serial.print("Date: ");
        Serial.print(gps.day, DEC); Serial.print('/');
        Serial.print(gps.month, DEC); Serial.print("/20");
        Serial.println(gps.year, DEC);
        Serial.print("Fix: "); Serial.print((int)gps.fix);
        Serial.print(" quality: "); Serial.println((int)gps.fixquality);
        if (gps.fix) {
          Serial.print("Location: ");
          Serial.print(gps.latitude, 4); Serial.print(gps.lat);
          Serial.print(", ");
          Serial.print(gps.longitude, 4); Serial.println(gps.lon);
          Serial.print("Speed (knots): "); Serial.println(gps.speed);
          Serial.print("Angle: "); Serial.println(gps.angle);
          Serial.print("Altitude: "); Serial.println(gps.altitude);
          Serial.print("Satellites: "); Serial.println((int)gps.satellites);
          Serial.print("Antenna status: "); Serial.println((int)gps.antenna);
        }
      }
      

      if(gps.fix && gps.day != 0){
        //On a réussi a tout avoir
        break;
      }
    }
  }

  //Recupération des données
  D(Serial.println("Recuperation des donnees gps"));
  ds.m.latitude = gps.latitude_fixed;
  ds.m.longitude = gps.longitude_fixed;
  ds.m.hdop = gps.HDOP;
  ds.m.nbSat = gps.satellites;

  tmElements_t tm;
  tm.Hour   = gps.hour;
  tm.Minute = gps.minute;
  tm.Second = gps.seconds;
  tm.Day    = gps.day;
  tm.Month  = gps.month;
  tm.Year   = gps.year + 30; // On veur le nombre d'années depuis 1970 et gps.year donne les 2 derniers digits => gps.year + 2000 - 1970
  D(Serial.println(tm.Year));
  D(Serial.println(((int)gps.year) + 2000));
  unsigned long gpsEpoch = makeTime(tm); // Change the tm structure into time_t (seconds since epoch)

  if(PRINT_DEBUG){
    // Get RTC epoch time
    unsigned long rtcEpoch = rtc.getEpoch();

    // Calculate RTC drift
    long rtcDrift = rtcEpoch - gpsEpoch;

    Serial.println("\t- gpsEpoch: " + String(gpsEpoch));
    Serial.println("\t- rtcEpoch: " + String(rtcEpoch));
    Serial.println("\t- rtcDrift: " + String(rtcDrift));
    Serial.println("\t- unixtime: " + String(unixtime));
  }

  //Sync RTC
  D(Serial.println("Synchronisation du RTC"));
  rtc.setTime(gpsEpoch);
  D("\t> Success");
  
  return true;
}



bool sendSAT(DataStruct &ds){ //À TESTER
  enable5V();
  
  bool retVal = false;
  SerialSatGps.end();
  
  //Active le switch sur le bon appareil
  digitalWrite(P_S0, HIGH);
  digitalWrite(P_S1, LOW);

  //Put data in struct
  MSG_SAT msgSat;
  msgSat.unixtime            = ds.m.timestamp; //uint32_t  
  msgSat.temperatureInt      = ds.m.tempInt * 100;   //int16_t   
  msgSat.humidityInt         = ds.m.humInt * 100;    //uint16_t  
  msgSat.pressureExt         = ds.m.pressExt - 400 * 100;  //uint16_t  
  msgSat.temperatureExt      = ds.m.tempExt * 100;   //int16_t   
  msgSat.humidityExt         = ds.m.humExt * 100;    //uint16_t  
  msgSat.pitch               = ds.m.accelX * 100;    //int16_t   à confirmer l'axe
  msgSat.roll                = ds.m.accelY * 100;    //int16_t   à confirmer l'axe
  msgSat.solar               = ds.m.lum * 10000;       //uint32_t  
  msgSat.windSpeed           = ds.m.vitVent * 100;   //uint16_t  
  msgSat.windDirection       = ds.m.dirVent * 10;   //uint16_t  
  msgSat.windGustSpeed       = 0; //uint16_t  EUX C'EST QUOI?
  msgSat.windGustDirection   = 0; //uint16_t  EUX C'EST QUOI?
  msgSat.latitude            = ds.m.latitude/10; //* 1000000;  //int32_t   
  msgSat.longitude           = ds.m.longitude/10; //* 1000000; //int32_t   
  msgSat.satellites          = ds.m.nbSat;   //uint8_t   
  msgSat.hauteurNeige        = 0;            //uint16_t  
  msgSat.voltage             = ds.m.vBat * 100;      //uint16_t  
  msgSat.transmitDuration    = 0; //uint16_t  EUX C'EST QUOI?
  msgSat.transmitStatus      = 0; //uint8_t   EUX C'EST QUOI?
  msgSat.iterationCounter    = ds.m.iteration; //uint16_t  

  if(PRINT_DEBUG){
    Serial.println("--- DataStruct ---");
    Serial.println("\tUnixtime  :" + String(ds.m.timestamp));
    Serial.println("\tTempInt   :" + String(ds.m.tempInt));     
    Serial.println("\tHumInt    :" + String(ds.m.humInt));        
    Serial.println("\tPressExt  :" + String(ds.m.pressExt));        
    Serial.println("\tTempExt   :" + String(ds.m.tempExt));     
    Serial.println("\tHumExt    :" + String(ds.m.humExt));        
    Serial.println("\tPitch     :" + String(ds.m.accelX));              
    Serial.println("\tRoll      :" + String(ds.m.accelY));               
    Serial.println("\tLum       :" + String(ds.m.lum));              
    Serial.println("\tWindSp    :" + String(ds.m.vitVent));          
    Serial.println("\tWindDr    :" + String(ds.m.dirVent));      
    Serial.println("\tGustSpeed :" + String(-99));
    Serial.println("\tGustDir   :" + String(-99));//
    Serial.println("\tLatitude  :" + String(ds.m.latitude));           
    Serial.println("\tLongitude :" + String(ds.m.longitude));          
    Serial.println("\tSatellite :" + String(-99));
    Serial.println("\tHautNeige :" + String(-99));       
    Serial.println("\tVolt      :" + String(ds.m.vBat));            
    Serial.println("\tDTransmit :" + String(-99));
    Serial.println("\tTransStat :" + String(-99));
    Serial.println("\tIteration :" + String(ds.m.iteration));  

    Serial.println("--- Data SBD ---");
    Serial.println("\tUnixtime  :" + String(msgSat.unixtime));
    Serial.println("\tTempInt   :" + String(msgSat.temperatureInt));     
    Serial.println("\tHumInt    :" + String(msgSat.humidityInt));        
    Serial.println("\tPressExt  :" + String(msgSat.pressureExt));        
    Serial.println("\tTempExt   :" + String(msgSat.temperatureExt));     
    Serial.println("\tHumExt    :" + String(msgSat.humidityExt));        
    Serial.println("\tPitch     :" + String(msgSat.pitch));              
    Serial.println("\tRoll      :" + String(msgSat.roll));               
    Serial.println("\tLum       :" + String(msgSat.solar));              
    Serial.println("\tWindSp    :" + String(msgSat.windSpeed));          
    Serial.println("\tWindDr    :" + String(msgSat.windDirection));      
    Serial.println("\tGustSpeed :" + String(msgSat.windGustSpeed));
    Serial.println("\tGustDir   :" + String(msgSat.windGustDirection));//
    Serial.println("\tLatitude  :" + String(msgSat.latitude));           
    Serial.println("\tLongitude :" + String(msgSat.longitude));          
    Serial.println("\tSatellite :" + String(msgSat.satellites));
    Serial.println("\tHautNeige :" + String(msgSat.hauteurNeige));       
    Serial.println("\tVolt      :" + String(msgSat.voltage));            
    Serial.println("\tDTransmit :" + String(msgSat.transmitDuration));
    Serial.println("\tTransStat :" + String(msgSat.transmitStatus));
    Serial.println("\tIteration :" + String(msgSat.iterationCounter));   
  }

  //Assigne le bon Baudrate
  SerialSatGps.begin(config.sat.baud, SERIAL_8N1, P_TX_SW, P_RX_SW);
  delay(100);

  //Initialisation ou wakeup du sat
  D(Serial.println("Starting modem Iridium..."));
  int retCode = modemSat.begin();

  //Verify the presence of Modem
  if(retCode != ISBD_SUCCESS){
    if(retCode == ISBD_NO_MODEM_DETECTED){
      D(Serial.println("\t! Aucun modem trouve, verifier les branchements"));
    }
    else{
      D(Serial.println("\t! Failed to initialize modem, error: " + String(retCode)));
    } 
  }
  else{
    // This returns a number between 0 and 5.
    // 2 or better is preferred.
    int signalQuality = -1;
    retCode = modemSat.getSignalQuality(signalQuality);
    if (retCode != ISBD_SUCCESS)
    {
      D(Serial.print("SignalQuality failed: error "));
      D(Serial.print(retCode));
      D(Serial.print(", Quality: "));
      D(Serial.println(signalQuality));
    }
    else{
      // Send the message
      D(Serial.print("Trying to send the message.  This might take several minutes.\r\n"));
      retCode = modemSat.sendSBDBinary(msgSat.bytes, sizeof(msgSat.bytes));
      if (retCode != ISBD_SUCCESS)
      {
        D(Serial.print("sendSBDBinary failed: error "));
        D(Serial.println(retCode));
        if (retCode == ISBD_SENDRECEIVE_TIMEOUT)
          Serial.println("Try again with a better view of the sky.");
      }
      else
      {
        D(Serial.println("Hey, it worked!"));
        retVal = true;
      }
    }
  }

  //Putting modem to sleep
  modemSat.sleep();
  disable5V();
  return retVal;
}
