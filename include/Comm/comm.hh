#include <pico/stdlib.h>
#include <hardware/uart.h>
#include <uart.hh>
#include <protocol.hh>

#ifndef COMM_HPP
#define COMM_HPP

#define UART_ID uart0
#define UART_BAUD_RATE 9600
#define UART_DATA_BITS 8
#define UART_STOP_BITS 1
#define UART_PARITY UART_PARITY_NONE

#define UART_TX 0
#define UART_RX 1

#define PACKET_SIZE 3

class Comm {
  private:
    Protocol* _comm_proto;
    uint8_t _packet_size;
    uint8_t* _received_data;
    bool _there_is_data;
  public:
    Comm(Protocol*, uint8_t);
    void send_data(uint8_t*);
    uint8_t* read_data();
    bool there_is_data();
    void data_received();
};

#endif
