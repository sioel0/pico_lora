#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <cstdint>
#include <protocol.hh>

#ifndef UART_HH
#define UART_HH

#define UART_ID uart0
#define TX_PIN 0
#define RX_PIN 1
#define BAUD_RATE 9600

#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

class Uart : public Protocol {
  private:
    uart_inst_t *_uart;
    uint _br;
    uint _tx;
    uint _rx;
    uint _data_bits;
    uint _stop_bits;
    uart_parity_t _parity;

    bool led_on = false;
  public:
    Uart(
      uart_inst_t *uart = UART_ID,
      uint br = BAUD_RATE,
      uint tx = TX_PIN,
      uint rx = RX_PIN,
      uint data_bits = DATA_BITS,
      uint stop_bits = STOP_BITS,
      uart_parity_t parity = PARITY
    );
    void send_data(uint8_t*, uint8_t) override;
    void read_data(uint8_t*, uint8_t) override;
    bool there_is_data() override;
};

#endif
