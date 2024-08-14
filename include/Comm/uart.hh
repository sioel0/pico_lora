#include <cstdint>
#include <protocol.hh>
#include <hardware/uart.h>
#include <pico/stdlib.h>

#ifndef UART_HH
#define UART_HH

#define UART_ID uart0
#define UART_BAUD_RATE 9600
#define UART_DATA_BITS 8
#define UART_STOP_BITS 1
#define UART_PARITY UART_PARITY_NONE

#define UART_TX_PIN 0
#define UART_RX_PIN 1

class Uart : public Protocol {
  private:
    uart_inst_t *_uart;
    uint _tx;
    uint _rx;
    uint _br;
    uint _data_bits;
    uint _stop_bits;
    uart_parity_t _parity;
  public:
    Uart(
      uart_inst_t* uart = UART_ID,
      uint tx = UART_TX_PIN,
      uint rx = UART_RX_PIN,
      uint br = UART_BAUD_RATE,
      uint data_bits = UART_DATA_BITS,
      uint stop_bits = UART_STOP_BITS,
      uart_parity_t parity = UART_PARITY
    );
    void send_data(uint8_t*, uint8_t) override;
    void read_data(uint8_t*, uint8_t) override;
    bool there_is_data() override;
};

#endif
