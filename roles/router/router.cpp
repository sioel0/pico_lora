#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <cstdio>
#include <RadioLib.h>
#include <PicoHal.h>
#include <comm.hh>
#include <uart.hh>
#include <protocol.hh>

#define PACKET_SIZE 3

#define SPI_PORT spi1
#define SPI_MISO 12
#define SPI_MOSI 11
#define SPI_SCK 10

#define CS_PIN  3 // chip select pin
#define INT_PIN 20 // interrupt pin
#define RST_PIN 15 // reset pin
#define BUSY_PIN 2 // busy pin which is optional but used on 1262

void lora_main() {
  PicoHal* hal = new PicoHal(SPI_PORT, SPI_MISO, SPI_MOSI, SPI_SCK);
  SX1262 radio = new Module(hal, CS_PIN, INT_PIN, RST_PIN, BUSY_PIN);
  uint8_t lora_data[PACKET_SIZE] = {0x12, 0x34, 0x56};

  radio.begin();

  while(true) {
    int state = radio.transmit(lora_data, PACKET_SIZE);
    if(state == RADIOLIB_ERR_NONE) {
      printf("LoRa sent\n");
    } else {
      printf("LoRa error: %d\n", state);
    }
    sleep_ms(4000);
  }
}

int main() {
  stdio_init_all();
  Comm comm = Comm(new Uart(), PACKET_SIZE);
  uint8_t data[PACKET_SIZE] = {0x12, 0x34, 0x56};

  multicore_launch_core1(lora_main);

  while(true) {
    comm.send_data(data);
    printf("Uart sent\n");
    sleep_ms(2000);
  }
    return 0;
}
