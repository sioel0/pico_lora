#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <comm.h>
#include <stdio.h>

static uint8_t mcu_received_data[3];
static uint8_t data_received = 0;

void on_uart_rx() {
  if(uart_is_readable(UART_INTERFACE))
    comm_data_received(1);
}

void comm_init() {
  uart_init(UART_INTERFACE, BAUD_RATE);
  
  gpio_set_function(UART_TX, UART_FUNCSEL_NUM(UART_INTERFACE, UART_TX));
  gpio_set_function(UART_RX, UART_FUNCSEL_NUM(UART_INTERFACE, UART_RX));

  uart_set_hw_flow(UART_INTERFACE, false, false);

  uart_set_format(UART_INTERFACE, DATA_BITS, STOP_BITS, PARITY_BIT);

  uart_set_fifo_enabled(UART_INTERFACE, false);

  int UART_IRQ = UART_INTERFACE == uart0 ? UART0_IRQ : UART1_IRQ;

  irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
  irq_set_enabled(UART_IRQ, true);

  uart_set_irq_enables(UART_INTERFACE, true, false);
  printf("Initialized\n");
}

void comm_send_data(uint8_t *data) {
  uart_write_blocking(UART_INTERFACE, data, PACKET_SIZE);
  printf("Sent: 0x%x 0x%x 0x%x\n", data[0], data[1], data[2]);
}

uint8_t* comm_read_data() {
  uart_read_blocking(UART_INTERFACE, mcu_received_data, PACKET_SIZE);
  return mcu_received_data;
}

uint8_t comm_there_is_data() {
  return data_received;
}

void comm_data_received(uint8_t recv) {
  data_received = recv;
}
