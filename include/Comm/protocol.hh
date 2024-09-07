/**
 * @file protocol.hh
 * @author sioel0
 * @brief This file contains the virtual class which must be implemented to create a serial communication abstraction layer
 */

#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <stdint.h>

/**
 * @class Protocol
 * @brief virtual class which represent the basic structure for the serial communication abstraction layer
 */
class Protocol {

  public:
/**
 * @brief This function is used to send data with the serial communication protocol
 * @param source a buffer containing the data that will be sent
 * @param size the \p source length
 */
    virtual void send_data(uint8_t* source, uint8_t size) = 0;

/**
 * @brief This function is used to read data from the serial communication protocol
 * @param dest a buffer to store the received data
 * @param size the amount of data that will be read
 */
    virtual void read_data(uint8_t* dest, uint8_t size) = 0;

/**
 * @brief this function is used to check if there is any data on the receive buffer of the serial interface
 */
    virtual bool there_is_data() = 0;

    virtual ~Protocol() {}
};

#endif /* PROTOCOL_HPP */
