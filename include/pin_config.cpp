#include "pin_config.h"
#include "driver/uart.h"

void Pin::configurePins()
{
  uart_set_pin(UART_RS485, TX_RS485, RX_RS485, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  uart_set_pin(UART_SW, TX_SWITCH, RX_SWITCH, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}