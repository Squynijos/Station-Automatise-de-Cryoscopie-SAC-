//Pour le Modbus

void initRS485(){ //Fonctionnelle
  pinMode(P_RE, INPUT);
  pinMode(P_DE, OUTPUT);
  modbus.begin(config.mb.baud, SERIAL_8N1, P_RX_485, P_TX_485);
  modbus.setTimeout(config.mb.timeout);
}

uint16_t readVitVent(DataStruct &ds){ //Fonctionnelle
  D(Serial.println("Reading VitVent"));

  uint8_t tryCount = 0;
  uint16_t errMB = 0;
  uint16_t reg[1] = {0};

  //Tentative de lecture des données
  do{
    D(Serial.println("\t- Loop"));
    modbus.readHoldingRegisters(ADDR_ANEMO, REG_VIT, reg, 1);
    errMB = modbus.getExceptionResponse(); //A RETIRER si on update la lib
    modbus.clearExceptionResponse(); //A RETIRER si on update la lib
    //Y a t-il une erreur?
    if(errMB){
      D(Serial.println("\t! Erreur modbus DirVent: " + String(errMB)));
      tryCount++;
      delay(config.mb.retryDelai);
    }
  }
  while(errMB && tryCount < config.mb.maxRetry);

  //Est-ce qu'on a réssi?
  if(!errMB){
    D(Serial.println("\t> Data: " + String(reg[0])));
    D(Serial.println("\t> Erreur: " + String(errMB)));
    ds.m.vitVent = reg[0];
  }
  return errMB;
  // uint16_t reg[1] = {0};
  // modbus.readHoldingRegisters(ADDR_ANEMO, REG_VIT, reg, 1);
  // ds.m.vitVent = reg[0];
}

uint16_t readDirVent(DataStruct &ds){ //À TESTER, puis copier sur les autres
  D(Serial.println("Reading DirVent"));

  uint8_t tryCount = 0;
  uint16_t errMB = 0;
  uint16_t reg[2] = {0, 0};

  //Tentative de lecture des données
  do{
    D(Serial.println("\t- Loop"));
    modbus.readHoldingRegisters(ADDR_GIROU, REG_ANGLE, reg, 2);
    errMB = modbus.getExceptionResponse(); //A RETIRER si on update la lib
    modbus.clearExceptionResponse(); //A RETIRER si on update la lib
    //Y a t-il une erreur?
    if(errMB){
      D(Serial.println("\t! Erreur modbus: " + String(errMB)));
      tryCount++;
      delay(config.mb.retryDelai);
    }
  }
  while(errMB && tryCount < config.mb.maxRetry);

  //Est-ce qu'on a réssi?
  if(!errMB){
    D(Serial.println("\t> Data: " + String(reg[0]) +", "+ String(reg[1])));
    D(Serial.println("\t> Erreur: " + String(errMB)));
    ds.m.angleVent = reg[0];
    ds.m.dirVent   = reg[1];
  }
  return errMB;
}

uint16_t readBmeExt(DataStruct &ds){ //Fonctionnelle
  D(Serial.println("Reading BME280"));

  uint8_t tryCount = 0;
  uint16_t errMB = 0;
  uint16_t reg[3] = {0, 0, 0};

  //Tentative de lecture des données
  do{
    D(Serial.println("\t- Loop"));
    modbus.readHoldingRegisters(ADDR_BME_EXT, REG_HUM, reg, 3);
    errMB = modbus.getExceptionResponse(); //A RETIRER si on update la lib
    modbus.clearExceptionResponse(); //A RETIRER si on update la lib
    //Y a t-il une erreur?
    if(errMB){
      D(Serial.println("\t! Erreur modbus: " + String(errMB)));
      tryCount++;
      delay(config.mb.retryDelai);
    }
  }
  while(errMB && tryCount < config.mb.maxRetry);

  //Est-ce qu'on a réssi?
  if(!errMB){
    D(Serial.println("\t> Data: " + String(reg[0]) +", "+ String(reg[1]) +", "+ String(reg[2])));
    D(Serial.println("\t> Erreur: " + String(errMB)));
    ds.m.humExt     = reg[0]/10.0;
    ds.m.tempExt    = reg[1]/10.0;
    ds.m.pressExt   = reg[2]/10.0;
  }
  return errMB;
  // D(Serial.println("Reading BME280"));

  // uint16_t reg[3] = {0, 0, 0};
  // modbus.readHoldingRegisters(ADDR_BME_EXT, REG_HUM, reg, 3);
  // ds.m.humExt     = reg[0]/10.0;
  // ds.m.tempExt    = reg[1]/10.0;
  // ds.m.pressExt   = reg[2]/10.0;
}

uint16_t readLum(DataStruct &ds){ //Fonctionnelle
  D(Serial.println("Reading Lum"));

  uint8_t tryCount = 0;
  uint16_t errMB = 0;
  uint16_t reg[1] = {0};

  //Tentative de lecture des données
  do{
    D(Serial.println("\t- Loop"));
    modbus.readHoldingRegisters(ADDR_LUX, REG_LUM, reg, 1);
    errMB = modbus.getExceptionResponse(); //A RETIRER si on update la lib
    modbus.clearExceptionResponse(); //A RETIRER si on update la lib
    //Y a t-il une erreur?
    if(errMB){
      D(Serial.println("\t! Erreur modbus DirVent: " + String(errMB)));
      tryCount++;
      delay(config.mb.retryDelai);
    }
  }
  while(errMB && tryCount < config.mb.maxRetry);

  //Est-ce qu'on a réssi?
  if(!errMB){
    D(Serial.println("\t> Data: " + String(reg[0])));
    D(Serial.println("\t> Erreur: " + String(errMB)));
    ds.m.lum = reg[0];
  }
  return errMB;
  // uint16_t reg[1] = {0};
  // modbus.readHoldingRegisters(ADDR_LUX, REG_LUM, reg, 1);
  // ds.m.lum = reg[0];
}