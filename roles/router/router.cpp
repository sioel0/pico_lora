#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <cstdio>
#include <RadioLib.h>
#include <PicoHal.h>
#include <comm.hh>
#include <usb.hh>
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

uint8_t input_buff[PACKET_SIZE][BUFFER_SIZE];
uint8_t input_read_idx = 0, input_write_idx = 0;
auto_init_recursive_mutex(input_mutex);

uint8_t output_buff[PACKET_SIZE][BUFFER_SIZE];
uint8_t output_read_idx = 0, output_write_idx = 0;
auto_init_recursive_mutex(output_mutex);

void lora_main() {
  PicoHal* hal = new PicoHal(SPI_PORT, SPI_MISO, SPI_MOSI, SPI_SCK);
  SX1262 radio = new Module(hal, CS_PIN, INT_PIN, RST_PIN, BUSY_PIN);
  uint8_t input_lora_data[PACKET_SIZE];
  uint8_t output_lora_data[PACKET_SIZE];

  int i = 0, att = 0;

  radio.begin();

  while(true) {
    // lora input
    int state = radio.receive(input_lora_data, PACKET_SIZE);
    if(state == RADIOLIB_ERR_NONE) {
      recursive_mutex_enter_blocking(&input_mutex);
      {
        for(i = 0; i < PACKET_SIZE; ++i) {
          input_buff[i][input_write_idx] = input_lora_data[i];
        }
        input_write_idx++;
        if(input_write_idx == BUFFER_SIZE) {
          input_write_idx = 0;
        }
      }
      recursive_mutex_exit(&input_mutex);
    }
    // lora output
    recursive_mutex_enter_blocking(&output_mutex);
    {
      att = 0;
      // there is data that needs to be sent
      if(output_read_idx == output_read_idx - 1 || 
        (output_read_idx == BUFFER_SIZE - 1 && output_write_idx == 0)) {
        for(i = 0; i < PACKET_SIZE; ++i) {
          output_lora_data[i] = output_buff[i][output_read_idx];
        }
        do {
          radio.transmit(output_lora_data, PACKET_SIZE);
        } while(state != RADIOLIB_ERR_NONE && att != MAX_TRY);
        // move on the read idx only if data has been actually sent
        if(state == RADIOLIB_ERR_NONE) {
          output_read_idx++;
        }
        if(output_read_idx == BUFFER_SIZE) {
          output_read_idx = 0;
        }
      }
    }
    recursive_mutex_exit(&output_mutex);
  }
}

int main() {
  stdio_init_all();
  uint8_t* input_usb_data;
  uint8_t output_usb_data[PACKET_SIZE];
  Comm* comm = new Comm(new Usb(), PACKET_SIZE);

  int i = 0;

  multicore_launch_core1(lora_main);

  while(true) {
    // usb input
    input_usb_data = comm->read_data();
    if(input_usb_data[0] != 0x00) {
      recursive_mutex_enter_blocking(&output_mutex);
      {
        for(i = 0; i < PACKET_SIZE; ++i) {
          output_buff[i][output_write_idx] = input_usb_data[i];
        }
        output_write_idx++;
        if(output_write_idx == BUFFER_SIZE) {
          output_write_idx = 0;
        }
      }
      recursive_mutex_exit(&output_mutex);
    }
    // usb output
    recursive_mutex_enter_blocking(&input_mutex);
    {
      if(input_read_idx == input_write_idx - 1 ||
        (input_read_idx == BUFFER_SIZE - 1 && input_write_idx == 0)) {
        for(i = 0; i < PACKET_SIZE; ++i) {
          output_usb_data[i] = input_buff[i][input_read_idx];
        }
        input_read_idx++;
        comm->send_data(output_usb_data);
        if(input_read_idx == BUFFER_SIZE) {
          input_read_idx = 0;
        }
      }
    }
    recursive_mutex_exit(&input_mutex);
  }
    return 0;
}
