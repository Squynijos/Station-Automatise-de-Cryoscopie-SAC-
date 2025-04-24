//Pour les capteurs internes

void initI2C(){ //Fonctionnelle
  if(!bme.begin(ADDR_BME_INT)){
    Serial.println("Erreur avec BME");
  }

  lsm.disableMagnetometer();
  lsm.enableAccelerometer();
}

void readBmeInt(DataStruct &ds){ //Fonctionnelle
  ds.m.tempInt  = bme.readTemperature();
  ds.m.humInt   = bme.readHumidity();
  ds.m.pressInt = bme.readPressure();
}

void readMagAccel(DataStruct &ds){ //Fonctionnelle
  //Magnétomètre
  // ds.m.magX = lsm.getMagX();
  // ds.m.magY = lsm.getMagY();
  // ds.m.magZ = lsm.getMagZ();

  //Accéléromètre
  ds.m.accelX = lsm.getX();
  ds.m.accelY = lsm.getY();
  ds.m.accelZ = lsm.getZ();
}
