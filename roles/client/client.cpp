#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <RadioLib.h>
#include <stdint.h>
#include <PicoHal.h>
#include <comm.hh>
#include <uart.hh>
#include <protocol.hh>
#include <hardware/watchdog.h>

#define ROUTER_ADDRESS 0x00
#define DEVICE_ADDRESS 0x01 // this can be any value between 0x01 and 0xFF

#define PACKET_SIZE 2
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

#define RESET_PIN 16

// used to store and manage data coming from the network
uint8_t input_buff[PACKET_SIZE][BUFFER_SIZE];
uint8_t input_read_idx = 0, input_write_idx = 0;
auto_init_recursive_mutex(input_mutex);

// used to store and manage data coming from mcu
uint8_t output_buff[PACKET_SIZE][BUFFER_SIZE];
uint8_t output_read_idx = 0, output_write_idx = 0;
auto_init_recursive_mutex(output_mutex);

static bool there_is_lora_data = false;
static bool transmit = false;
static uint8_t count = 0;
static bool initialized = false;

void software_reset() {
  watchdog_enable(1, 1); // enable 1ms watchdog
  while(1); // busy wait for reboot
}

void gpio_irq_callback(uint gpio, uint32_t events) {
  if(gpio == RESET_PIN) {
    software_reset();
  }
}

void lora_input() {
  // since the interrupt routine is the same both for I/O we have to make sure
  // that we are processing data only when we are actually receiving it
  if(!transmit) {
    if(count == 0) {
      there_is_lora_data = true;
    }
  }
  // for some reason the interrupt routine is called twice for each packet received
  // this is necessary to do not process the same packet twice
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
    initialized = true;
  } else {
    initialized = false;
  }

  while(true) {
    // manage lora input
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
    // manage lora output
    recursive_mutex_enter_blocking(&output_mutex); 
    {
      if(output_read_idx < output_write_idx || 
        (output_read_idx != 0 && output_write_idx == 0)) {
        output_lora_data[0] = ROUTER_ADDRESS;
        output_lora_data[1] = DEVICE_ADDRESS;
        for(i = 0; i < PACKET_SIZE; ++i) {
          output_lora_data[i + LORA_PACKET_SIZE - PACKET_SIZE] = output_buff[i][output_read_idx];
        }
        transmit = true;
        state = radio.startTransmit(output_lora_data, LORA_PACKET_SIZE);
        // move on the read idx only if data has been sent
        if(state == RADIOLIB_ERR_NONE) {
          output_read_idx++;
        }
        sleep_ms(200); // this is necessary to give lora device enough time to send data before switching back to receive mode
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
  uint8_t* input_uart_data;
  uint8_t output_uart_data[PACKET_SIZE];
  Comm *comm = new Comm(new Uart(), PACKET_SIZE);

  gpio_init(RESET_PIN);
  gpio_set_dir(RESET_PIN, GPIO_IN);
  gpio_set_irq_enabled_with_callback(RESET_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &gpio_irq_callback);

  int i = 0;

  multicore_launch_core1(lora_main);

  while(true) {
    // uart input
    input_uart_data = comm->read_data(); 
    if(input_uart_data[0] != 0x00) {
      recursive_mutex_enter_blocking(&output_mutex);
      {
          for(i = 0; i < PACKET_SIZE; ++i) {
            output_buff[i][output_write_idx] = input_uart_data[i];
          }
          output_write_idx++;
          if(output_write_idx == BUFFER_SIZE) {
            output_write_idx = 0;
          }
      }
      recursive_mutex_exit(&output_mutex);
    }
    // uart output
    recursive_mutex_enter_blocking(&input_mutex);
    {
      if(input_read_idx < input_write_idx ||
        (input_read_idx != 0 && input_write_idx == 0)) {
        for(i = 0; i < PACKET_SIZE; ++i) {
          output_uart_data[i] = input_buff[i][input_read_idx];
        }
        input_read_idx++;
        comm->send_data(output_uart_data);
        if(input_read_idx == BUFFER_SIZE) {
          input_read_idx = 0;
        }
      }
    }
    recursive_mutex_exit(&input_mutex);
  }
  return 0;
}
