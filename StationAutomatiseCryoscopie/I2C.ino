//Pour les capteurs internes

void initI2C(){
  if(!bme.begin(ADDR_BME_INT)){
    Serial.println("Erreur avec BME");
  }

  lsm.enableMagnetometer();
  lsm.enableAccelerometer();
}

void readBmeInt(DataStruct &dataStruct){ //À TESTER
  dataStruct.m.tempInt  = bme.readTemperature();
  dataStruct.m.humInt   = bme.readHumidity();
  dataStruct.m.pressInt = bme.readPressure();
}

void readMagAccel(DataStruct &dataStruct){ //À TESTER
  //Magnétomètre
  dataStruct.m.magX = lsm.getMagX();
  dataStruct.m.magY = lsm.getMagY();
  dataStruct.m.magZ = lsm.getMagZ();

  //Accéléromètre
  dataStruct.m.accelX = lsm.getX();
  dataStruct.m.accelY = lsm.getY();
  dataStruct.m.accelZ = lsm.getZ();
}