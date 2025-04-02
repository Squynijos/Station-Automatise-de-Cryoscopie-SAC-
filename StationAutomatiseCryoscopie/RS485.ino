
bool initRS485(){
  SerialRS485.begin(BAUD_RS485);
  modbus.begin(BAUD_RS485);


  return true;
}

void readVitVent(){

}

void readDirVent(){

}

void readExtBME(){
  uint16_t inputRegisters[5];
  uint8_t error = modbus.readInputRegisters(ADDR_BME_EXT, 0, inputRegisters, 5);
  for(int i = 0; i<5; i++){

  Serial.println(inputRegisters[i]);
  }
  digitalWrite(LED_BUILTIN, HIGH);
}