/**
  * @file comm.hh
  * @author sioel0
  * @version 1.0.0
  * @brief This file contains all the high level abstraction of the communication module
  *
  */

#ifndef LIB_HH
#define LIB_HH

#include <hardware/uart.h>
#include <protocol.hh>

/**
 * @class Comm
  * @brief High level abstraction layer of the serial communication module
  */
class Comm {
  private:
/** 
  * @brief Pointer to the low level abstraction layer of the communication module
  */
    Protocol *_proto; 
/**
  * @brief packet size
  * Since the communication module has been built upon the assumption that all
  * the packets are going to follow a specific protocol with fixed sized packets
  */
    uint8_t _packet_size;
/**
  * @brief buffer which stores all the received data from the serial interface
  */
    uint8_t* _mcu_received_data;
/**
  * @brief buffer used as an indicator that no data has been received
  */
    uint8_t* _NO_DATA_AVAILABLE;
  public:
/**
  * @brief Class constructor
  * @param proto a pointer to a hardware communication protocol abstraction class
  * @param packet_size of the communication protocol defined for the overall application
  */
    Comm(Protocol* proto, uint8_t packet_size);
/**
  * @brief This function is used to send data via the serial interface
  * @param data the data that will be sent
  * The assumption for the @p data parameter is that the length is the same assumption
  * @ref Comm.
  */
    void send_data(uint8_t* data);
/**
  * @brief This function is used to read the serial interface received data
  * @return a pointer to the serial interface received data
  * This function return value may be either a pointer to the actual data or, to avoid
  * making this function blocking, if any data has been received, a pointer to an array
  * filled with 0x00. The returned data length will be exactly the same as @refe Comm packet  size.
  */
    uint8_t* read_data();
};

#endif
