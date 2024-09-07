#ifndef USB_HH
#define USB_HH

#include <protocol.hh>
#include <cstdint>

#define READ_TIMEOUT 100 * 1000 /** max usb read timeout 100ms*/
#define MAX_BUFF_SIZE 10 /** maximum data buffer size */

/**
 * @brief data reception callback function
 */
void received_data(void*);

/**
 * @class Usb
 * @brief usb abstraction layer for the serial communication
 */
class Usb : public Protocol {
  public:
/**
  * @brief class constructor
  */
    Usb();

/**
 * @brief This function is used to send data with the serial communication protocol
 * @param source a buffer containing the data that will be sent
 * @param size the \p source length
 */
    void send_data(uint8_t*, uint8_t) override;

/**
 * @brief This function is used to read data from the serial communication protocol
 * @param dest a buffer to store the received data
 * @param size the amount of data that will be read
 */
    void read_data(uint8_t*, uint8_t) override;

/**
 * @brief this function is used to check if there is any data on the receive buffer of the serial interface
 */
    bool there_is_data() override;
};

#endif
