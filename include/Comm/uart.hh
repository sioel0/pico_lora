/**
 * @file uart.hh
 * @author sioel0
 * @brief This file contains the Uart abstraction layer implementation
 */

#ifndef UART_HH
#define UART_HH

#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <cstdint>
#include <protocol.hh>

#define UART_ID uart0 /** default uart interface */
#define TX_PIN 0 /** default tx pin */
#define RX_PIN 1 /** default rx pin */
#define BAUD_RATE 9600 /** default baudrate */

#define DATA_BITS 8 /** default data bits number */
#define STOP_BITS 1 /** default number of stop bits */
#define PARITY UART_PARITY_NONE /** default parity bit setting */

/**
  * @class Uart
  * @brief Uart abstraction layer
  * This class is an implementation of the \ref Protocol virtual class,
  * this class can be used with the \ref Comm class to create a full communication
  * protocol abstraction stack.
  */
class Uart : public Protocol {

  private:
    uart_inst_t *_uart; /** uart interface */
    uint _br; /** communication baudrate */
    uint _tx; /** uart tx pin */
    uint _rx; /** uart rx pin */
    uint _data_bits; /** number of data bits */
    uint _stop_bits; /** number of stop bits */
    uart_parity_t _parity; /** parity bit setting */

  public:
/**
  * @brief @ref Uart constructor
  * @param uart pointer to the uart interface that will be used
  * @param br baudrate that will be used for the communication
  * @param tx tx pin for the uart interface of choice
  * @param rx rx pin for the uart interface of choice
  * @param data_bits number of data bits that will be used in uart communication, it can be  in the interval [5, 9]
  * @param stop_bits number of stop bits that will be used for the communication, it can be either 1 or 2
  * @param parity thte parity option which can be any of uart_parity_t defined in <a href="https://www.raspberrypi.com/documentation/pico-sdk/hardware.html#enumerations-14">pico-sdk</a> 
  */
    Uart(
      uart_inst_t *uart = UART_ID,
      uint br = BAUD_RATE,
      uint tx = TX_PIN,
      uint rx = RX_PIN,
      uint data_bits = DATA_BITS,
      uint stop_bits = STOP_BITS,
      uart_parity_t parity = PARITY
    );

/**
 * @brief This function is used to send data with the serial communication protocol
 * @param source a buffer containing the data that will be sent
 * @param size the \p source length
 */
    void send_data(uint8_t*, uint8_t) override;

/**
 * @brief This function is used to read data from the serial communication protocol
 * @param dest a buffer to store the received data
 * @param size the amount of data that will be read
 */
    void read_data(uint8_t*, uint8_t) override;

/**
 * @brief this function is used to check if there is any data on the receive buffer of the serial interface
 */
    bool there_is_data() override;
};

#endif
