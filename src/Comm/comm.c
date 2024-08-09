#include <hardware/i2c.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <pico/i2c_slave.h>
#include <comm.h>
#include <stdio.h>

uint8_t mcu_received_data[3];
uint8_t data_received = 0;

static void slave_handler(i2c_inst_t *i2c, i2c_slave_event_t event) {
  printf("Received data\n");
  switch(event) {
    case I2C_SLAVE_RECEIVE:
      received_data(1);
      printf("Size: %d\n", i2c_get_read_available(i2c));
      break;
    case I2C_SLAVE_REQUEST:
      break;
    case I2C_SLAVE_FINISH:

      break;
    default:
      break;
  }
}

void init_master() {
  gpio_set_function(I2C_MASTER_SDA, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_MASTER_SDA);
  gpio_set_function(I2C_MASTER_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_MASTER_SCL);

  if(i2c_init(I2C_MASTER, I2C_BAUD_RATE) == I2C_BAUD_RATE)
    printf("Master initialized\n");
}

void init_slave() {
  gpio_set_function(I2C_SLAVE_SDA, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SLAVE_SDA);
  gpio_set_function(I2C_SLAVE_SCL, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SLAVE_SCL);

  if(i2c_init(I2C_SLAVE, I2C_BAUD_RATE) == I2C_BAUD_RATE)
    printf("Slave initialized\n");
  i2c_slave_init(I2C_SLAVE, I2C_SLAVE_ADDR, &slave_handler);
}

void send_data(uint8_t *data) {
  printf("Trying to send data\n");
  i2c_write_blocking(I2C_MASTER, I2C_SLAVE_ADDR, data, PACKET_SIZE, false);
  printf("Sent 0x%x 0x%x 0x%x", data[0], data[1], data[2]);
}

void read_data() {
  while(i2c_get_read_available(I2C_SLAVE) < PACKET_SIZE) {}
  i2c_read_raw_blocking(I2C_SLAVE, mcu_received_data, PACKET_SIZE);
  printf("Data read\n");
}

const uint8_t* get_received_data() {
  return mcu_received_data;
}

uint8_t there_is_data() {
  return data_received;
}

void received_data(uint8_t received) {
  data_received = received;
}
