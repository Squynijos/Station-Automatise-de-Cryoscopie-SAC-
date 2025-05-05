// //Pour le communication avec le gps et le SAT

void initUART(){ //À TESTER
  //Les pins pour le select
  pinMode(P_S0, OUTPUT);
  pinMode(P_S1, OUTPUT);
  digitalWrite(P_S0, LOW);
  digitalWrite(P_S1, LOW);

  //Default to gps
  SerialSatGps.begin(config.gps.baud, SERIAL_8N1, P_RX_SW, P_TX_SW);
  delay(10);
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA); //Use RMC and GGA
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ); //Update rate = 1Hz

  //Configure Satellite
  modemSat.setPowerProfile(IridiumSBD::DEFAULT_POWER_PROFILE);     // Assume battery power (USB power: IridiumSBD::USB_POWER_PROFILE)
  modemSat.adjustSendReceiveTimeout(config.sat.timeout);           // Timeout for Iridium send/receive commands (default = 300 s)
  modemSat.adjustStartupTimeout(config.sat.timeout / 2);           // Timeout for Iridium startup (default = 240 s)
}

bool readGPS(DataStruct &ds){ //À TESTER
  D(Serial.println("Communication avec gps"));

  //Active ls switch sur le bon appareil
  D(Serial.println("\t- Selecting Port"));
  digitalWrite(P_S0, LOW);
  digitalWrite(P_S1, LOW);
  SerialSatGps.begin(config.gps.baud, SERIAL_8N1, P_RX_SW, P_TX_SW);

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
      return false;
    }

    char c = gps.read();
      
    //On décode le message recu
    if(gps.newNMEAreceived()){
      char* s = gps.lastNMEA();
      D(Serial.println("\t- Last NMEA: " + String(s)));
      if(!gps.parse(s)){
        D(Serial.println("\t! Parsing error"));
        continue;
      }
      if(DEBUG){
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

  if(DEBUG){
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
  //Active ls switch sur le bon appareil
  digitalWrite(P_S0, LOW);
  digitalWrite(P_S1, HIGH);

  //Activate power on sat module
  modem.power(true);

  //Assigne le bon Baudrate
  SerialSatGps.begin(config.sat.baud, SERIAL_8N1, P_RX_SW, P_TX_SW);

  //Initialisation
  D(Serial.println("Starting le modem Iridium"));
  int retCode = modemSat.begin();

  if(retCode != ISBD_SUCCESS){
    if(retCode == ISBD_NO_MODEM_DETECTED){
      D(Serial.println("\t! Aucun modem trouve, verifier les branchements"));
    }
    else{
      D(Serial.println("\t! Failed to initialize modem, error: " + String(retCode)));
    } 
    disable5V();
    return false;
  }

  // This returns a number between 0 and 5.
  // 2 or better is preferred.
  int signalQuality = -1;
  retCode = modem.getSignalQuality(signalQuality);
  if (retCode != ISBD_SUCCESS)
  {
    Serial.print("SignalQuality failed: error ");
    Serial.println(retCode);
    disable5V();
    return false;
  }

  //Writing Values
  //TODO

  // Send the message
  Serial.print("Trying to send the message.  This might take several minutes.\r\n");
  err = modem.sendSBDBinary(msgSat, sizeof(msgSat));
  if (err != ISBD_SUCCESS)
  {
    D(Serial.print("sendSBDBinary failed: error "));
    Serial.println(err);
    if (err == ISBD_SENDRECEIVE_TIMEOUT)
      Serial.println("Try again with a better view of the sky.");
  }
  else
  {
    Serial.println("Hey, it worked!");
  }

  //Putting modem to sleep
  modem.power(false);

  //Closing UART Port
  //SerialSatGps.end();
  return true;
}
