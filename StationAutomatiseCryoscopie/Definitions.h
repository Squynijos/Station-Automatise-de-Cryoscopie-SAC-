#pragma once

//---------- Broches ------------
  //UART
#define P_S1 26
#define P_S0 25
#define P_RX_SW 16
#define P_TX_SW 17
#define P_TX_485 4
#define P_TX_485 12
#define P_TX_DBG 1
#define P_RX_DBG 3
  //I2C
#define P_SCL 22
#define P_SDA 21
  //SPI
#define P_MISO 19
#define P_MOSI 23
#define P_SCK 18
#define P_CS_SD 10
  //PIN ANALOG
#define P_35 35
#define P_34 34
#define P_39 39
  //DEL RGB
#define P_DEL_B 8
#define P_DEL_R 7
#define P_DEL_G 6
  //ON-OFF
#define P_SHDN_12 2
#define P_SHDN_5 5
#define P_SHDN_3V3 13
#define P_SAT 14
  //RS485
#define P_DE 9
#define P_RE 27
  //AUTRES
#define P_3DFIX 0
#define P_VBAT 15

//---------- Adresses -----------
#define ADDR_BME 0x76

//---------- Flags --------------
#define DEBUG true

//---------- Macro --------------
#if DEBUG
  #define D(...) (__VA_ARGS__)
#else
  #define D(...) (void(0))
#endif

//---------- Renaming -----------


