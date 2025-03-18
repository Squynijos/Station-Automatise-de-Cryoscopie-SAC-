#pragma once
#include "pins_arduino.h"
#include "driver/uart.h"

//--------- PINS DEF ---------
#define UART_DBG UART_NUM_0
#define UART_RS485 UART_NUM_2
#define UART_SW UART_NUM_2

const int TX_DEBUG   = 1;
const int RX_DEBUG   = 3;
const int TX_SWITCH  = 17;
const int RX_SWITCH  = 16;
const int TX_RS485   = 4;
const int RX_RS485   = 12;
const int ON_OFF_SAT = 14;
// const int SCL        = 22;
// const int SDA        = 21;
// const int MISO       = 19;
// const int MOSI       = 23;
// const int SCK        = 18;
const int FIX_3D     = 0;
const int SUP_BAT    = 15;
const int DEL_B      = 8;
const int DEL_R      = 7;
const int DEL_G      = 6;
const int SHDN_12V   = 2;
const int SHND_5V    = 5;
const int SW_3V3     = 13;
const int CS_SD      = 10;
const int DE         = 9;
const int RE         = 27;
const int S1         = 26;
const int S0         = 25;

/**
 * Classe contenant les fonctions de configuration des Pins
 */
class Pin
{
public:
  /**
   * Constucteur de logique des Pins
   */
  Pin();

  /**
   * Exécute la configuration hardware des pins du Firebeetle
   */
  static void configurePins();
};