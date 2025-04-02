
bool initI2C(){
  bme.begin(ADDR_BME);
  
  return true;
}

