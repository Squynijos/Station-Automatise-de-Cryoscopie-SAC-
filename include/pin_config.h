#pragma once
#include "pins_arduino.h"
#include "driver/uart.h"

//--------- PINS DEF ---------
#define UART_DBG UART_NUM_0
#define UART_RS485 UART_NUM_2
#define UART_SW UART_NUM_2

const int TX_DEBUG = 1;
const int RX_DEBUG = 3;
const int TX_SWITCH = 17;
const int RX_SWITCH = 16;
const int TX_RS485 = 4;
const int RX_RS485 = 12;

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
  void configurePins();
};