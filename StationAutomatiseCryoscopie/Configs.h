#pragma once

// Structures de config
struct ModbusConf{
  int baud            = 9600;
  uint8_t maxRetry    = 3;
  uint16_t retryDelai = 60; //ms
  uint16_t timeout    = 500; //ms
};

struct SatConf{
  int baud                    = 19200;
  uint8_t maxMsgLength        = 50; //bit
  uint8_t transmissionParJour = 24; //1t/h
  uint8_t timeout             = 240; //ms
};

struct GpsConf{
  int baud        = 9600;
  uint8_t timeout = 120; //ms
};

struct CapteurConf{ //Defaut = Aucune modif
  float offset  = 0;
  float facteur = 1;
};

struct Config{
  uint8_t acquisitionParHeure = 5; //5m/h
  
  ModbusConf mb;
  SatConf sat;
  GpsConf gps;

  CapteurConf lum;
  CapteurConf tempInt;
  CapteurConf humInt;
  CapteurConf pressInt;
  CapteurConf tempExt; //off: +0.5571  fact: 0.9941
  CapteurConf humExt; //off: -2.4027   fact: 1.0523
  CapteurConf pressExt;
  CapteurConf anemo;
  CapteurConf girou;

  // Constructor to set specific default values
  Config() {
    tempExt.offset = 0.5571f;
    tempExt.facteur = 0.9941f;

    humExt.offset = -2.4027f;
    humExt.facteur = 1.0523f;
  }
};