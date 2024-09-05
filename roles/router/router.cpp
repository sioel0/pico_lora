#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <cstdio>
#include <RadioLib.h>
#include <PicoHal.h>
#include <comm.hh>
#include <usb.hh>
#include <protocol.hh>

#define DEVICE_ADDRESS 0x00

#define PACKET_SIZE 3
#define LORA_PACKET_SIZE 4
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

static bool there_is_lora_data = false;
static bool transmit = false;
static uint8_t count = 0;

void lora_input() {
  if(!transmit) {
    if(count == 0) {
      there_is_lora_data = true;
    }
  }
  count++;
  if(count == 2) {
    count = 0;
    transmit = false;
  }
}

void lora_main() {
  PicoHal* hal = new PicoHal(SPI_PORT, SPI_MISO, SPI_MOSI, SPI_SCK);
  SX1262 radio = new Module(hal, CS_PIN, INT_PIN, RST_PIN, BUSY_PIN);

  uint8_t input_lora_data[LORA_PACKET_SIZE];
  uint8_t output_lora_data[LORA_PACKET_SIZE];

  int i = 0;

  int state = radio.begin();
  radio.setPacketReceivedAction(lora_input);
  state = radio.startReceive();

  if(state == RADIOLIB_ERR_NONE) {
    printf("Initialized\n");
  } else {
    printf("Init error: %d\n", state);
  }


  while(true) {
    // lora input
    if(there_is_lora_data) {
      there_is_lora_data = false;
      radio.readData(input_lora_data, LORA_PACKET_SIZE);
      if(input_lora_data[0] == DEVICE_ADDRESS) {
        recursive_mutex_enter_blocking(&input_mutex);
        {
          for(i = 0; i < PACKET_SIZE; ++i) {
            input_buff[i][input_write_idx] = input_lora_data[i + LORA_PACKET_SIZE - PACKET_SIZE];
          }
          input_write_idx++;
          if(input_write_idx == BUFFER_SIZE) {
            input_write_idx = 0;
          }
        }
        recursive_mutex_exit(&input_mutex);
      }
    }
    // lora output
    recursive_mutex_enter_blocking(&output_mutex);
    {
      // there is data that needs to be sent
      if(output_read_idx < output_write_idx || 
        (output_read_idx != 0 && output_write_idx == 0)) {
        output_lora_data[1] = DEVICE_ADDRESS;
        for(i = 0; i < PACKET_SIZE; ++i) {
          if(i == 0) {
            output_lora_data[i] = output_buff[i][output_read_idx];
          } else {
            output_lora_data[i + LORA_PACKET_SIZE - PACKET_SIZE] = output_buff[i][output_read_idx];
          }
        }
        transmit = true;
        state = radio.startTransmit(output_lora_data, LORA_PACKET_SIZE);
        // move on the read idx only if data has been sent
        if(state == RADIOLIB_ERR_NONE) {
          output_read_idx++;
          sleep_ms(200); // this is neccessary to give the lora device enough time to send the data without messing up when switching to receive mode
        }
        if(output_read_idx == BUFFER_SIZE) {
          output_read_idx = 0;
        }
        radio.startReceive();
      }
    }
    recursive_mutex_exit(&output_mutex);
  }
}

int main() {
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
      if(input_read_idx < input_write_idx ||
        (input_read_idx != 0 && input_write_idx == 0)) {
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
