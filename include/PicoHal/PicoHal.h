#ifndef PICO_HAL_H
#define PICO_HAL_H

// include RadioLib
#include <RadioLib.h>

// include the necessary Pico libraries
#include "hardware/spi.h"

// create a new Raspberry Pi Pico hardware abstraction 
// layer using the Pico SDK
// the HAL must inherit from the base RadioLibHal class
// and implement all of its virtual methods
class PicoHal : public RadioLibHal {
public:
  PicoHal(spi_inst_t*, uint32_t, uint32_t, uint32_t, uint32_t speed = 500 * 1000);

  void init() override;
  void term() override;

  // GPIO-related methods (pinMode, digitalWrite etc.) should check
  // RADIOLIB_NC as an alias for non-connected pins
  void pinMode(uint32_t, uint32_t) override;
  void digitalWrite(uint32_t, uint32_t) override;
  uint32_t digitalRead(uint32_t) override;

  void attachInterrupt(uint32_t, void (*)(void), uint32_t) override;
  void detachInterrupt(uint32_t) override;

  void delay(unsigned long) override;
  void delayMicroseconds(unsigned long) override;

  unsigned long millis() override;
  unsigned long micros() override;

  long pulseIn(uint32_t, uint32_t, unsigned long) override;

  void spiBegin();
  void spiBeginTransaction();
  void spiTransfer(uint8_t*, size_t, uint8_t*);
  void spiEndTransaction();
  void spiEnd();

private:
  // the HAL can contain any additional private members
  spi_inst_t *_spiChannel;
  uint32_t _spiSpeed;
  uint32_t _misoPin;
  uint32_t _mosiPin;
  uint32_t _sckPin;
};

#endif
