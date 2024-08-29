#include <stdint.h>

#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

class Protocol {
  public:

    virtual void send_data(uint8_t*, uint8_t) = 0;
    virtual void read_data(uint8_t*, uint8_t) = 0;
    virtual bool there_is_data() = 0;

    virtual ~Protocol() {}
};

#endif /* PROTOCOL_HPP */
