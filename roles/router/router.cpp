#include <pico/stdlib.h>
#include <cstdio>
//#include <RadioLib.h>
//#include <PicoHal.h>
#include <comm.hh>
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

/*PicoHal* hal = new PicoHal(SPI_PORT, SPI_MISO, SPI_MOSI, SPI_SCK);*/
/**/
/*SX1262 radio = new Module(hal, CS_PIN, INT_PIN, RST_PIN, BUSY_PIN);*/

Comm comm = Comm(new Uart(), PACKET_SIZE);

int main() {
    /*printf("[SX1262] Initializing router ... ");*/
    /*int state = radio.begin();*/
    /*if(state != RADIOLIB_ERR_NONE) {*/
    /*    printf("failed, code %d\n", state);*/
    /*    return 1;*/
    /*}*/
    /*printf("success!\n");*/
  stdio_init_all();
  uint8_t data[PACKET_SIZE] = {0x12, 0x34, 0x56};

  while(true) {
    comm.send_data(data);
    sleep_ms(2000);
    //send a packet
    /*printf("[SX1262] Receiving packet .. ");*/
    /*uint8_t buff[13];*/
    /*state = radio.receive(buff, 13);*/
    /*if(state == RADIOLIB_ERR_NONE) {*/
    /*    printf("%s\n", buff);*/
    /**/
    /*    hal->delay(1000);*/
    /*} else {*/
    /*    printf("failed, code %d\n", state);*/
    /*}*/
  }
    return 0;
}
