#include <comm.hh>
#include <protocol.hh>
#include <hardware/uart.h>
#include <pico/stdlib.h>
#include <cstdint>

Comm::Comm(Protocol *proto, uint8_t packet_size) :
  _proto(proto),
  _packet_size(packet_size) {

  _NO_DATA_AVAILABLE = new uint8_t[packet_size];
  _mcu_received_data = new uint8_t[packet_size];
  for(int i = 0; i < packet_size; ++i) {
    _NO_DATA_AVAILABLE[i] = 0x00;
  }
}

void Comm::send_data(uint8_t *data) {
  _proto->send_data(data, _packet_size);
}

uint8_t* Comm::read_data() {
  if(_proto->there_is_data()) {
    _proto->read_data(_mcu_received_data, _packet_size);
    return _mcu_received_data;
  }
  return _NO_DATA_AVAILABLE;
}
