#include <hardware/uart.h>
#include <protocol.hh>

#ifndef LIB_HH
#define LIB_HH

class Comm {
  private:
    Protocol *_proto;
    uint8_t _packet_size;
    uint8_t* _mcu_received_data;
    uint8_t* _NO_DATA_AVAILABLE;
  public:
    Comm(Protocol*, uint8_t);
    void send_data(uint8_t*);
    uint8_t* read_data();
};

#endif
