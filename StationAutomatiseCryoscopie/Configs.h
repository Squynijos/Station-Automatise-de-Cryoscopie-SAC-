#pragma once

// Structures de config
struct ModbusConf{
  int baud = 9600;
  uint8_t maxRetry = 3;
  uint16_t retryDelai = 60; //ms
  uint16_t timeout = 500; //ms
};

struct SatConf{
  uint8_t maxMsgLength = 50; //bit
  uint8_t transmissionParJour = 24; //1t/h
};

struct CapteurConf{ //Defaut = Aucune modif
  float offset = 0;
  float facteur = 1;
};

struct Config{
  uint8_t acquisitionParHeure = 5; //5m/h
  
  ModbusConf mb;
  SatConf sat;

  CapteurConf lum;
  CapteurConf tempInt;
  CapteurConf humInt;
  CapteurConf pressInt;
  CapteurConf tempExt;
  CapteurConf humExt;
  CapteurConf pressExt;
  CapteurConf anemo;
  CapteurConf girou;
};