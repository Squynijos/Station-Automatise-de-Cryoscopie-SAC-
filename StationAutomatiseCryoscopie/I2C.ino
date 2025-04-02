
bool initI2C(){
  bme.begin(ADDR_BME);
  Serial.println("nice");
  return true;
}

