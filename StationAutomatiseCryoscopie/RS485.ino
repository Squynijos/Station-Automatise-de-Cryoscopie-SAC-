//Pour le Modbus

void initRS485(){ //Fonctionnelle
  pinMode(P_RE, INPUT);
  pinMode(P_DE, OUTPUT);
  modbus.begin(BAUD_RS485, SERIAL_8N1, P_RX_485, P_TX_485);
  modbus.setTimeout(MB_TIMEOUT);
}

void readVitVent(DataStruct &ds){ //À TESTER
  uint16_t reg[1] = {0};
  modbus.readHoldingRegisters(ADDR_ANEMO, REG_VIT, reg, 1);
  ds.m.vitVent = reg[0];
}

void readDirVent(DataStruct &ds){ //À TESTER
  uint16_t reg[2] = {0, 0};
  modbus.readHoldingRegisters(ADDR_GIROU, REG_ANGLE, reg, 2);
  ds.m.angleVent = reg[0];
  ds.m.dirVent   = reg[1];
}

void readBmeExt(DataStruct &ds){ //Fonctionnelle
  uint16_t reg[3] = {0, 0, 0};
  modbus.readHoldingRegisters(ADDR_BME_EXT, REG_HUM, reg, 3);
  ds.m.humExt     = reg[0]/10.0;
  ds.m.tempExt    = reg[1]/10.0;
  ds.m.pressExt   = reg[2]/10.0;
}

void readLum(DataStruct &ds){ //À TESTER
  uint16_t reg[1] = {0};
  modbus.readHoldingRegisters(ADDR_LUX, REG_LUM, reg, 1);
  ds.m.lum = reg[0];
}