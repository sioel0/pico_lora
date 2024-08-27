#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <RadioLib.h>
#include <stdint.h>
#include <PicoHal.h>
#include <comm.hh>
#include <uart.hh>
#include <protocol.hh>

#define PACKET_SIZE 3
#define BUFFER_SIZE 10
#define MAX_TRY 5

#define SPI_PORT spi1
#define SPI_MISO 12
#define SPI_MOSI 11
#define SPI_SCK 10

#define CS_PIN  3 // chip select pin
#define INT_PIN 20 // interrupt pin
#define RST_PIN 15 // reset pin
#define BUSY_PIN 2 // busy pin which is optional but used on 1262

// used to store and manage data coming from the network
uint8_t input_buff[PACKET_SIZE*BUFFER_SIZE];
uint8_t input_read_idx = 0, input_write_idx = 0;
auto_init_recursive_mutex(input_mutex);

// used to store and manage data coming from mcu
uint8_t output_buff[PACKET_SIZE*BUFFER_SIZE];
uint8_t output_read_idx = 0, output_write_idx = 0;
auto_init_recursive_mutex(output_mutex);

void lora_main() {
  PicoHal* hal = new PicoHal(SPI_PORT, SPI_MISO, SPI_MOSI, SPI_SCK);
  SX1262 radio = new Module(hal, CS_PIN, INT_PIN, RST_PIN, BUSY_PIN);
  uint8_t* input_lora_data = new uint8_t[PACKET_SIZE];
  uint8_t* output_lora_data = new uint8_t[PACKET_SIZE];

  radio.begin();

  while(true) {
    // manage lora input
    int state = radio.receive(input_lora_data, PACKET_SIZE);
    if(state == RADIOLIB_ERR_NONE) {
      if(input_lora_data[0] != 0x00) {
        recursive_mutex_enter_blocking(&input_mutex);
        {
          for(int i = 0; i < PACKET_SIZE; ++i) {
            input_buff[input_write_idx] = input_lora_data[i];
            input_write_idx++;
          }
          if(input_write_idx == BUFFER_SIZE * PACKET_SIZE) {
            input_write_idx = 0;
          }
        }
        recursive_mutex_exit(&input_mutex);
      }
    }
    // manage lora output
    recursive_mutex_enter_blocking(&output_mutex); 
    {
      int att = 0;
      if(output_read_idx == output_write_idx - PACKET_SIZE || 
        (output_read_idx == (BUFFER_SIZE - 1) * PACKET_SIZE && output_write_idx == 0)) {
        for(int i = 0; i < PACKET_SIZE; ++i) {
          output_lora_data[i] = output_buff[output_read_idx];
          output_read_idx++;
        }
        do {
          radio.transmit(output_lora_data, PACKET_SIZE);
          att++;
        } while(state != RADIOLIB_ERR_NONE && att != MAX_TRY);
        if(output_read_idx == BUFFER_SIZE * PACKET_SIZE) {
          output_read_idx = 0;
        }
      }
    }
    recursive_mutex_exit(&output_mutex);
  }
}

void uart_main(Comm *comm) {
}

int main() {
  stdio_init_all();
  uint8_t* input_uart_data;
  uint8_t* output_uart_data = new uint8_t[PACKET_SIZE];
  Comm *comm = new Comm(new Uart(), PACKET_SIZE);

  multicore_launch_core1(lora_main);

  while(true) {
    // manage uart input
    recursive_mutex_enter_blocking(&output_mutex);
    {
      input_uart_data = comm->read_data(); 
      // data has been received
      if(input_uart_data[0] != 0x00) {
        for(int i = 0; i < PACKET_SIZE; ++i) {
          output_buff[output_write_idx] = input_uart_data[i];
          output_write_idx++;
        }
        if(output_write_idx == BUFFER_SIZE * PACKET_SIZE) {
          output_write_idx = 0;
        }
      }
    }
    recursive_mutex_exit(&output_mutex);

    // manage uart output
    recursive_mutex_enter_blocking(&input_mutex);
    {
      if(input_read_idx == input_write_idx - 3 ||
        (input_read_idx == (BUFFER_SIZE - 1) * PACKET_SIZE && input_write_idx == 0)) {
        for(int i = 0; i < PACKET_SIZE; ++i) {
          output_uart_data[i] = input_buff[input_read_idx];
          input_read_idx++;
        }
        comm->send_data(output_uart_data);
        if(input_read_idx == BUFFER_SIZE * PACKET_SIZE) {
          input_read_idx = 0;
        }
      }
    }
    recursive_mutex_exit(&input_mutex);
  }
  return 0;
}
