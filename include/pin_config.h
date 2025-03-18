#pragma once
#include "pins_arduino.h"

class Pin
{
public:
#define TX_DEBUG 1
#define RX_DEBUG 3
#define TX_SWITCH 17
#define RX_SWITCH 16
#define TX_RS485 4
#define RX_RS485 12

  /**
   * Exécute la configuration hardware des pins du Firebeetle
   */
  static void configurePins();
};