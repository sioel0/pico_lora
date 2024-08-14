#include <uart.hh>
#include <hardware/uart.h>
#include <pico/stdlib.h>

Uart::Uart(
  uart_inst_t *uart,
  uint tx,
  uint rx,
  uint br,
  uint data_bits,
  uint stop_bits,
  uart_parity_t parity
) :
  _uart(uart),
  _tx(tx),
  _rx(rx),
  _br(br),
  _data_bits(data_bits),
  _stop_bits(stop_bits),
  _parity(parity) {

  // initialize uart instance
  uart_init(_uart, _br);

  gpio_set_function(_tx, UART_FUNCSEL_NUM(_uart, _tx));
  gpio_set_function(_rx, UART_FUNCSEL_NUM(_uart, _rx));
  
  int UART_IRQ = _uart == uart0 ? UART0_IRQ : UART1_IRQ;
  irq_set_exclusive_handler(UART_IRQ, on_data_received);
  irq_set_enabled(UART_IRQ, true);
  uart_set_irq_enables(_uart, true, false);
}

void Uart::send_data(uint8_t *src, uint8_t len) {
  uart_write_blocking(_uart, src, len);
}

void Uart::read_data(uint8_t *dst, uint8_t len) {
  uart_read_blocking(_uart, dst, len);
}

bool Uart::there_is_data() {
  return uart_is_readable(_uart);
}
