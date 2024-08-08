#include <hardware/i2c.h>
#include <pico/stdlib.h>
#include <stdint.h>
#include <pico/i2c_slave.h>

#ifndef COMM_H
#define COMM_H

#define I2C_SLAVE_SDA 4
#define I2C_SLAVE_SCL 5
#define I2C_MASTER_SDA 6
#define I2C_MASTER_SCL 7

#define I2C_MASTER i2c0
#define I2C_SLAVE i2c1

#define I2C_SLAVE_ADDR 0x22
#define I2C_MCU_ADDR 0x34

#define I2C_BAUD_RATE 100 * 1000

#define PACKET_SIZE 3

#ifdef __cplusplus
extern "C" {
#endif

static void slave_handler(i2c_inst_t*, i2c_slave_event_t);

void init_master();
void init_slave();
void send_data(uint8_t*);
void read_data();
const uint8_t* get_received_data();
uint8_t there_is_data();
void received_data(uint8_t);

#ifdef __cplusplus
}
#endif

#endif /* COMM_H */
