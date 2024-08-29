#include <protocol.hh>
#include <cstdint>

#ifndef USB_HH
#define USB_HH

#define READ_TIMEOUT 100 * 1000 // 100 ms
#define MAX_BUFF_SIZE 10

void received_data(void*);

class Usb : public Protocol {
  public:
    Usb();
    void send_data(uint8_t*, uint8_t) override;
    void read_data(uint8_t*, uint8_t) override;
    bool there_is_data() override;
};

#endif
