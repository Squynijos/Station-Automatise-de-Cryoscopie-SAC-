
bool initI2C(){
  bme.begin(ADDR_BME_INT);
  Serial.println("nice");
  return true;
}

