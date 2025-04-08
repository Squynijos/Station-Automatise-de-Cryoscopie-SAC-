//Pour les capteurs internes

void initI2C(){
  if(!bme.begin(ADDR_BME_INT)){
    Serial.println("Erreur avec BME");
  }

  lsm.enableMagnetometer();
  lsm.enableAccelerometer();
}