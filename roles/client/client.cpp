#include <pico/stdlib.h>
#include <RadioLib.h>
#include <stdint.h>
#include <PicoHal.h>
#include "comm.hh"
#include <uart.hh>
#include <protocol.hh>

#define SPI_PORT spi1
#define SPI_MISO 12
#define SPI_MOSI 11
#define SPI_SCK 10

#define CS_PIN  3
#define INT_PIN 20
#define RST_PIN 15
#define BUSY_PIN 2

#define PACKET_SIZE 3

PicoHal* hal = new PicoHal(SPI_PORT, SPI_MISO, SPI_MOSI, SPI_SCK);

SX1262 radio = new Module(hal, CS_PIN, INT_PIN, RST_PIN, BUSY_PIN);

Comm comm = Comm(new Uart(), PACKET_SIZE);

int main() {
  stdio_init_all();
  uint8_t* data;
  //printf("[SX1262] Initializing client ... ");
  //int state = radio.begin();
  //if(state != RADIOLIB_ERR_NONE) {
  //    printf("failed to initialize, code %d\n", state);
  //    return 1;
  //}
  //printf("success!\n");

  while(true) {
    data = comm.read_data();
    if(data[0] != 0x00) {
      printf("Received: 0x%x 0x%x 0x%x\n", data[0], data[1], data[2]);
    }
    //send a packet
    //printf("[SX1262] Transmitting packet .. ");
    //state = radio.transmit("Hello, World!");
    //if(state == RADIOLIB_ERR_NONE) {
    //    printf("success\n");

    //    hal->delay(1000);
    //} else {
    //    printf("failed to send, code %d\n", state);
    //}
  }
    return 0;
}
