// //Pour le communication avec le GPS et le SAT

// void initUART(){ //À TESTER
//   //Les pins pour le select
//   pinMode(P_S0, OUTPUT);
//   pinMode(P_S1, OUTPUT);
//   digitalWrite(P_S0, LOW);
//   digitalWrite(P_S1, LOW);

//   //Configure Satellite
//   modemSat.setPowerProfile(IridiumSBD::DEFAULT_POWER_PROFILE);     // Assume battery power (USB power: IridiumSBD::USB_POWER_PROFILE)
//   modemSat.adjustSendReceiveTimeout(config.sat.timeout);           // Timeout for Iridium send/receive commands (default = 300 s)
//   modemSat.adjustStartupTimeout(config.sat.timeout / 2);           // Timeout for Iridium startup (default = 240 s)
// }

// bool readGPS(DataStruct &ds){ //À TESTER
//   //Active ls switch sur le bon appareil
//   digitalWrite(P_S0, LOW);
//   digitalWrite(P_S1, LOW);

//   //Assigne le bon Baudrate
//   SerialSatGps.begin(config.gps.baud, SERIAL_8N1, P_RX_SW, P_TX_SW);
//   D(Serial.println("Communication avec GPS"));
//   delay(1000); //Nécessaire d'attendre au moins 1 sec après le démarrage du GPS

//   // Configuration
//   SerialSatGps.println("$PMTK220,1000*1F"); // Set NMEA update rate to 1 Hz
//   SerialSatGps.println("$PMTK314,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28"); // Set NMEA sentence output frequencies to GGA and RMC
//   //GNSS_PORT.println("$PGCMD,33,1*6C"); // Enable antenna updates
//   //GNSS_PORT.println("$PGCMD,33,0*6D"); // Disable antenna updates

//   //Listening for comm on Serial
//   unsigned long startTime = millis();
//   byte fixCounter = 0;

//   while (fixCounter < 10 && millis() - startTime < config.gps.timeout * 1000UL) {

//     if ((millis() - startTime) > 5000 && gps.charsProcessed() < 10) {
//       D(Serial.println("\t! Aucune donnee recue apres 5000ms"));
//       SerialSatGps.end();
//       return true;
//     }

//     //Rien reçu pour le moment
//     if (!SerialSatGps.available()){
//       continue;
//     }

//     //Reception d'un charactere
//     char c = SerialSatGps.read();
//     D(Serial.println("\t- Recu: " + c));
//     if(!gps.encode(c)){
//       D(Serial.println("\t- Character is not end of term"));
//       continue;
//     }

//     // Check if NMEA sentences have a valid fix and are not stale
//     // if (!((gpsFix.value() > 0 && gpsFix.age() < 1000) && (String(gpsValidity.value()) == "A" && gpsValidity.age() < 1000) && gps.satellites.value() > 0)){
//     //   continue;
//     // }  TODO

//     fixCounter++;
//     D(Serial.println("\t> Fix Increased: " + String(fixCounter)));
//   }
  
//   //Verifying reception
//   if(fixCounter < 10){
//     D(Serial.println("\t! Nombre de fix insuffisant avec timeout"));
//     SerialSatGps.end();
//     return false;
//   }

//   //Recupération des données
//   D(Serial.println("Recuperation des donnees GPS"));
//   ds.m.latitude = gps.location.lat();
//   ds.m.longitude = gps.location.lng();
//   //ds.m.satellites = gps.satellites.value();
//   ds.m.hdop = gps.hdop.value();

//   tmElements_t tm;
//   tm.Hour   = gps.time.hour();
//   tm.Minute = gps.time.minute();
//   tm.Second = gps.time.second();
//   tm.Day    = gps.date.day();
//   tm.Month  = gps.date.month();
//   tm.Year   = gps.date.year() - 1970; // Offset from 1970
//   unsigned long gpsEpoch = makeTime(tm); // Change the tm structure into time_t (seconds since epoch)

//   if(DEBUG){
//     // Get RTC epoch time
//     unsigned long rtcEpoch = rtc.getEpoch();

//     // Calculate RTC drift
//     long rtcDrift = rtcEpoch - gpsEpoch;

//     Serial.println("\t- gpsEpoch: " + String(gpsEpoch));
//     Serial.println("\t- rtcEpoch: " + String(rtcEpoch));
//     Serial.println("\t- rtcDrift: " + String(rtcDrift) + "s");
//   }

//   //Sync RTC
//   D(Serial.println("Synchronisation du RTC"));
//   if ((gpsEpoch > unixtime)) {
//     rtc.setEpoch(gpsEpoch);
//     D("\t> Success");
//   }
//   else {
//     D(Serial.println("! RTC sync failed. GPS time not accurate! "));
//     SerialSatGps.end();
//     return false;
//   }
  
//   //Closing UART Port
//   SerialSatGps.end();
//   return true;
// }

// bool sendSAT(DataStruct &ds){ //À TESTER
//   //Active ls switch sur le bon appareil
//   digitalWrite(P_S0, LOW);
//   digitalWrite(P_S1, HIGH);

//   //Activate power on sat module
//   enable5V();

//   //Assigne le bon Baudrate
//   SerialSatGps.begin(config.sat.baud, SERIAL_8N1, P_RX_SW, P_TX_SW);

//   //Initialisation
//   D(Serial.println("Starting le modem Iridium"));
//   int retCode = modemSat.begin();

//   if(retCode != ISBD_SUCCESS){
//     if(retCode == ISBD_NO_MODEM_DETECTED){
//       D(Serial.println("\t! Aucun modem trouve, verifier les branchement"));
//       return false;
//     }

//     D(Serial.println("\t! Failed to initialize modem, error: " + String(retCode)));
//     return false;
//   }

//   //Writing Values
//   //TODO

//   //Check if success
//   //TODO

//   //Putting modem to sleep
//   //TODO

//   //Closing UART Port
//   SerialSatGps.end();
//   return true;
// }
