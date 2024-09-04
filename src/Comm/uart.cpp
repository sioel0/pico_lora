#include <hardware/uart.h>
#include <cstdint>
#include <cstdio>
#include <pico/stdlib.h>
#include <uart.hh>
#include <protocol.hh>

Uart::Uart(uart_inst_t *uart, uint br, uint tx, uint rx, uint data_bits, uint stop_bits, uart_parity_t parity) :
  _uart(uart),
  _br(br),
  _tx(tx),
  _rx(rx),
  _data_bits(data_bits),
  _stop_bits(stop_bits),
  _parity(parity) {

  uart_init(_uart, _br);

  gpio_set_function(_tx, UART_FUNCSEL_NUM(_uart, _tx));
  gpio_set_function(_rx, UART_FUNCSEL_NUM(_uart, _rx));

  uart_set_hw_flow(UART_ID, false, false);
  
  uart_set_format(_uart, _data_bits, _stop_bits, _parity);
}

void Uart::send_data(uint8_t *data, uint8_t size) {
  uart_write_blocking(_uart, data, size);
}

void Uart::read_data(uint8_t *dest, uint8_t size) {
  uart_read_blocking(_uart, dest, size);
}

bool Uart::there_is_data() {
  return uart_is_readable(_uart);
}
