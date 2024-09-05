#include <usb.hh>
#include <cstdint>
#include <pico/stdlib.h>
#include <pico/stdio.h>
#include <cstdio>

static char buff[MAX_BUFF_SIZE];
static int idx = 0;
static bool data_ready = false;

void received_data(void* param) {
  char c = stdio_getchar_timeout_us(READ_TIMEOUT);
  if(c != PICO_ERROR_TIMEOUT) {
    printf("\n", c); 
    buff[idx] = c;
    idx++;
    data_ready = true;
  }
}

Usb::Usb() {
  stdio_usb_init();
  stdio_set_chars_available_callback(received_data, NULL);
}

void Usb::send_data(uint8_t* data, uint8_t size) {
  for(int i = 0; i < size; ++i) {
    printf("%2.2x", data[i]);
  }
  printf("\n");
}

void Usb::read_data(uint8_t* data, uint8_t size) {
  bool invalid_size = false;
  if(data_ready) {
    if(size > MAX_BUFF_SIZE || size > idx) {
      invalid_size = true;
    }
    for(int i = 0; i < size; ++i) {
      data[i] = invalid_size ? 0x00 : buff[i];
    }
    if(idx == 3) {
      data_ready = false;
      idx = 0;
    }
  }
}

bool Usb::there_is_data() {
  return idx > 0;
}
