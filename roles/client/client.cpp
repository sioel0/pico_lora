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

#define CLIENT

PicoHal* hal = new PicoHal(SPI_PORT, SPI_MISO, SPI_MOSI, SPI_SCK);

SX1262 radio = new Module(hal, CS_PIN, INT_PIN, RST_PIN, BUSY_PIN);

Comm comm = Comm(new Uart(), 3);

bool received = false;

void on_data_received() {
  received = !received;
}

int main() {
  stdio_init_all();
  sleep_ms(10000);
  gpio_init(PICO_DEFAULT_LED_PIN);
  gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
  //printf("[SX1262] Initializing client ... ");
  //int state = radio.begin();
  //if(state != RADIOLIB_ERR_NONE) {
  //    printf("failed to initialize, code %d\n", state);
  //    return 1;
  //}
  //printf("success!\n");

  while(true) {
    gpio_put(PICO_DEFAULT_LED_PIN, received);
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
