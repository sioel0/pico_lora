#include "PicoHal.h"

#include <RadioLib.h>

#include <pico/stdlib.h>
#include "hardware/spi.h"
#include "hardware/timer.h"

PicoHal::PicoHal(spi_inst_t *spiChannel, uint32_t misoPin, uint32_t mosiPin, uint32_t sckPin, uint32_t spiSpeed)
  : RadioLibHal(GPIO_IN, GPIO_OUT, 0, 1, GPIO_IRQ_EDGE_RISE, GPIO_IRQ_EDGE_FALL),
  _spiChannel(spiChannel),
  _spiSpeed(spiSpeed),
  _misoPin(misoPin),
  _mosiPin(mosiPin),
  _sckPin(sckPin) {
}

void PicoHal::init() {
  stdio_init_all();
  spiBegin();
}

void PicoHal::term() {
  spiEnd();
}

void PicoHal::pinMode(uint32_t pin, uint32_t mode) {
  if(pin == RADIOLIB_NC) {
    return;
  }

  gpio_init(pin);
  gpio_set_dir(pin, mode);
}

void PicoHal::digitalWrite(uint32_t pin, uint32_t value) {
  if(pin == RADIOLIB_NC) {
    return;
  }

  gpio_put(pin, (bool)value);
}

uint32_t PicoHal::digitalRead(uint32_t pin) {
  if(pin == RADIOLIB_NC) {
    return 0;
  }
  return gpio_get(pin);
}

void PicoHal::attachInterrupt(uint32_t interruptNum, void (*interruptCb)(void), uint32_t mode) {
  if(interruptNum == RADIOLIB_NC) {
    return;
  }

  gpio_set_irq_enabled_with_callback(interruptNum, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, (gpio_irq_callback_t)interruptCb);
}

void PicoHal::detachInterrupt(uint32_t interruptNum) {
  if(interruptNum == RADIOLIB_NC) {
    return;
  }

  gpio_set_irq_enabled_with_callback(interruptNum, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, false, NULL);
}

void PicoHal::delay(unsigned long ms) {
  sleep_ms(ms);
}

void PicoHal::delayMicroseconds(unsigned long us) {
  sleep_us(us);
}

unsigned long PicoHal::millis() {
  return to_ms_since_boot(get_absolute_time());
}

unsigned long PicoHal::micros() {
  return to_us_since_boot(get_absolute_time());
}

long PicoHal::pulseIn(uint32_t pin, uint32_t state, unsigned long timeout) {
  if(pin == RADIOLIB_NC) {
    return 0;
  }

  this->pinMode(pin, GPIO_IN);
  uint32_t start = this->micros();
  uint32_t curtick = this->micros();

  while(this->digitalRead(pin) == state) {
    if((this->micros() - curtick) > timeout) {
      return 0;
    }
  }

  return (this->micros() - start);
}

void PicoHal::spiBegin() {
  spi_init(_spiChannel, _spiSpeed);
  spi_set_format(_spiChannel, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

  gpio_set_function(_sckPin, GPIO_FUNC_SPI);
  gpio_set_function(_mosiPin, GPIO_FUNC_SPI);
  gpio_set_function(_misoPin, GPIO_FUNC_SPI);
}

void PicoHal::spiTransfer(uint8_t *out, size_t len, uint8_t *in) {
  spi_write_read_blocking(_spiChannel, out, in, len);
}

void PicoHal::spiEnd() {
  spi_deinit(_spiChannel);
}

void PicoHal::spiBeginTransaction() {}

void PicoHal::spiEndTransaction() {}
