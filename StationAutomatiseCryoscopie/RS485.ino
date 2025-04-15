//Pour le Modbus

void initRS485(){
  pinMode(P_RE, INPUT);
  pinMode(P_DE, OUTPUT);
  modbus.begin(BAUD_RS485, SERIAL_8N1, P_RX_485, P_TX_485);
}

void readVitVent(){
  uint16_t reg[1] = {0};
  modbus.readHoldingRegisters(ADDR_ANEMO, 0, reg, 1);
  Serial.println(reg[0]);
}

void readDirVent(){
  uint16_t reg[1] = {0};
  modbus.readHoldingRegisters(ADDR_GIROU, 0, reg, 1);
  Serial.println(reg[0]);
}

void readBmeExt(){
  uint16_t reg[1] = {0};
  modbus.readHoldingRegisters(ADDR_BME_EXT, 0, reg, 1);
  Serial.println(reg[0]);
}

void readLum(){
  uint16_t reg[1] = {0};
  modbus.readHoldingRegisters(ADDR_LUX, 0, reg, 1);
  Serial.println(reg[0]);
}