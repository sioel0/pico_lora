#include <comm.hh>
#include <protocol.hh>
#include <pico/stdlib.h>
#include <cstdio>

Comm::Comm(Protocol* proto, uint8_t packet_size) :
  _comm_proto(proto),
  _packet_size(packet_size),
  _there_is_data(false) {
}

void Comm::send_data(uint8_t *data) {
  _comm_proto->send_data(data, _packet_size);
}

uint8_t* Comm::read_data() {
  if(_comm_proto->there_is_data()) {
    _comm_proto->read_data(_received_data, _packet_size);
    return _received_data;
  }
  return NULL;
}

void Comm::data_received() {
  _there_is_data = true;
}

bool Comm::there_is_data() {
  return _there_is_data;
}
